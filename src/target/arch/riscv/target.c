#include "pt/pt.h"
#include "pt/timer.h"
#include "rvl-target-config.h"
#include "dm.h"
#include "dmi.h"
#include "encoding.h"
#include "rvl-target.h"
#include "rvl-jtag.h"

#ifndef RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM
#define RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM       4
#endif


#ifndef RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM
#define RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM       32
#endif


typedef union riscv_csr_mcontrol_u
{
    rvl_target_reg_t reg;
    struct {
        unsigned int load: 1;
        unsigned int store: 1;
        unsigned int execute: 1;
        unsigned int u: 1;
        unsigned int s: 1;
        unsigned int reserved5: 1;
        unsigned int m: 1;
        unsigned int match: 4;
        unsigned int chain: 1;
        unsigned int action: 4;
        unsigned int sizelo: 2;
        unsigned int timing: 1;
        unsigned int select: 1;
        unsigned int hit: 1;
        unsigned int maskmax: 6;
        unsigned int dmode: 1;
        unsigned int type: 4;
    };
}riscv_csr_mcontrol_t;

typedef union riscv_csr_dcsr_u
{
    uint32_t word;
    struct {
        unsigned int prv: 2;
        unsigned int step: 1;
        unsigned int nmip: 1;
        unsigned int mprven: 1;
        unsigned int reserved5: 1;
        unsigned int cause: 3;
        unsigned int stoptime: 1;
        unsigned int stopcount: 1;
        unsigned int stepie: 1;
        unsigned int ebreaku: 1;
        unsigned int ebreaks: 1;
        unsigned int reserved14: 1;
        unsigned int ebreakm: 1;
        unsigned int reserved16: 12;
        unsigned int xdebugver: 4;
    };
}riscv_csr_dcsr_t;


#define RISCV_CSR_DCSR_CAUSE_EBREAK                 1
#define RISCV_CSR_DCSR_CAUSE_TRIGGER                2
#define RISCV_CSR_DCSR_CAUSE_HALT_REQ               3
#define RISCV_CSR_DCSR_CAUSE_STEP                   4
#define RISCV_CSR_DCSR_CAUSE_RESET_HALT_REQ         5


#define RISCV_AAMSIZE_8BITS                         0
#define RISCV_AAMSIZE_16BITS                        1
#define RISCV_AAMSIZE_32BITS                        2
#define RISCV_AAMSIZE_64BITS                        3
#define RISCV_AAMSIZE_128BITS                       4


typedef struct riscv_breakpoint_s
{
    rvl_target_breakpoint_type_t type;
    rvl_target_addr_t addr;
    int kind;
}riscv_hardware_breakpoint_t;

typedef struct riscv_software_breakpoint_s
{
    rvl_target_breakpoint_type_t type;
    rvl_target_addr_t addr;
    int kind;
    uint32_t orig_inst;
}riscv_software_breakpoint_t;

typedef struct riscv_target_s
{
    struct pt pt;
    struct pt pt_sub;
    struct timer timer;
    riscv_dm_t dm;
    uint32_t dmi_result;
    uint32_t i;
    uint32_t ii;
    riscv_csr_dcsr_t dcsr;
    rvl_dtm_idcode_t idcode;
    rvl_dtm_dtmcs_t dtmcs;
    rvl_target_reg_t tselect;
    riscv_csr_mcontrol_t mcontrol;
    rvl_target_reg_t dpc;
    rvl_dmi_reg_t dmi_data;
    uint32_t dmi_op;
    const char *err_msg;
    uint32_t err_pc;
    riscv_hardware_breakpoint_t hardware_breakpoints[RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM];
    riscv_software_breakpoint_t software_breakpoints[RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM];
    uint32_t wp_inst;
    uint32_t wp_addr_base_regno;
    rvl_target_addr_t wp_addr_offset;
}riscv_target_t;

static riscv_target_t riscv_target_i;
#define self riscv_target_i


static PT_THREAD(riscv_read_register(rvl_target_reg_t* reg, uint32_t regno));
static PT_THREAD(riscv_write_register(rvl_target_reg_t reg, uint32_t regno));
static PT_THREAD(riscv_read_mem(uint8_t* mem, rvl_target_addr_t addr, size_t len, int aamsize));
static PT_THREAD(riscv_write_mem(const uint8_t* mem, rvl_target_addr_t addr, size_t len, int aamsize));
static void riscv_parse_watchpoint_inst(uint32_t inst, uint32_t* regno, uint32_t* offset);


const char* riscv_abstractcs_cmderr_str[8] = {
        "abs:0 (none)",
        "abs:1 (busy)",
        "abs:2 (not supported)",
        "abs:3 (exception)",
        "abs:4 (halt/resume)",
        "abs:5 (bus)",
        "abs:6 (FIXME)",
        "abs:7 (other)",
};

void riscv_target_init(void)
{
    PT_INIT(&self.pt);
    PT_INIT(&self.pt_sub);

    for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
        self.hardware_breakpoints[self.i].type = rvl_target_breakpoint_type_unused;
    }

    for(self.i = 0; self.i < RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM; self.i++) {
        self.software_breakpoints[self.i].type = rvl_target_breakpoint_type_unused;
    }

    rvl_target_clr_error();

    rvl_dmi_init();
}


PT_THREAD(riscv_target_init_post(rvl_target_error_t *err))
{
    PT_BEGIN(&self.pt);

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13

    PT_WAIT_THREAD(&self.pt, rvl_dtm_idcode(&self.idcode));
    if(self.idcode.word == 0x00000000 || self.idcode.word == 0xffffffff) {
        *err = rvl_target_error_line;
        PT_EXIT(&self.pt);
    }

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs(&self.dtmcs));
    if(self.dtmcs.version != 1) {
        *err = rvl_target_error_compat;
        PT_EXIT(&self.pt);
    }

    /*
     * Pulse dmcontrol.dmactive low to get the Debug Module into a known state.
     */
    self.dm.dmcontrol.reg = 0;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));
    if(self.dmi_result != RISCV_DMI_RESULT_DONE) {
        *err = rvl_target_error_debug_module;
        PT_EXIT(&self.pt);
    }

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));
    if(self.dmi_result != RISCV_DMI_RESULT_DONE) {
        *err = rvl_target_error_debug_module;
        PT_EXIT(&self.pt);
    }

    if(self.dm.dmcontrol.dmactive != 0) {
        *err = rvl_target_error_debug_module;
        PT_EXIT(&self.pt);
    }

    /*
     * Active debug module
     */
    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.dmactive = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));
    if(self.dmi_result != RISCV_DMI_RESULT_DONE) {
        *err = rvl_target_error_debug_module;
        PT_EXIT(&self.pt);
    }

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));
    if(self.dmi_result != RISCV_DMI_RESULT_DONE) {
        *err = rvl_target_error_debug_module;
        PT_EXIT(&self.pt);
    }

    if(self.dm.dmcontrol.reg != 1) {
        *err = rvl_target_error_debug_module;
        PT_EXIT(&self.pt);
    }

#else
#error
#endif

    *err = rvl_target_error_none;
    PT_END(&self.pt);
}


PT_THREAD(riscv_target_init_after_halted(rvl_target_error_t *err))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
    if(self.dcsr.xdebugver != 4) {
        *err = rvl_target_error_compat;
        PT_EXIT(&self.pt);
    }

    /*
     * ebreak instructions in X-mode enter Debug Mode.
     */
    self.dcsr.ebreakm = 1;
    self.dcsr.ebreaks = 1;
    self.dcsr.ebreaku = 1;
    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr.word, CSR_DCSR));

    /*
     * clear all hardware breakpoints
     */
    for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
        PT_WAIT_THREAD(&self.pt, riscv_write_register(self.i, CSR_TSELECT));
        PT_WAIT_THREAD(&self.pt, riscv_write_register(0, CSR_TDATA1));
    }

    PT_END(&self.pt);
}


PT_THREAD(riscv_target_fini_pre(void))
{
    PT_BEGIN(&self.pt);

    /*
     * ebreak instructions in X-mode behave as described in the Privileged Spec.
     */
    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
    self.dcsr.ebreakm = 0;
    self.dcsr.ebreaks = 0;
    self.dcsr.ebreaku = 0;
    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr.word, CSR_DCSR));

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13

    /*
     * Disable debug module
     */
    self.dm.dmcontrol.reg = 0;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

#else
#error
#endif

    PT_END(&self.pt);
}


void riscv_target_fini(void)
{
    rvl_dmi_fini();
}


void rvl_target_set_error(const char *str)
{
    uint32_t pc;
    asm volatile (
            "mv %0, ra\n"
            :"=r"(pc)
            );
    self.err_pc = pc;
    self.err_msg = str;
}


void rvl_target_get_error(const char **str, uint32_t* pc)
{
    *str = self.err_msg;
    *pc = self.err_pc;
}


void rvl_target_clr_error(void)
{
    self.err_pc = 0;
    self.err_msg = "no error";
}


uint32_t riscv_target_get_idcode(void)
{
    return self.idcode.word;
}


PT_THREAD(rvl_target_read_core_registers(rvl_target_reg_t *regs))
{
    PT_BEGIN(&self.pt);

    regs[0] = 0x0;

#if 1 // optimize
#if defined(RVL_TARGET_CONFIG_HAS_ABS_CMD_AUTO) && defined(RVL_TARGET_CONFIG_HAS_AAR_POST_INC)
    // Set Abstract Command Autoexec
    self.dm.abstractauto.reg = 0;
    self.dm.abstractauto.autoexecdata = 0x01;

    self.dmi_data = self.dm.abstractauto.reg;
    self.dmi_op = RISCV_DMI_OP_WRITE;
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_ABSTRACT_AUTO, &self.dmi_data, &self.dmi_op));

    self.dm.command_access_register.reg = 0;
    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.aarpostincrement = 1;
    self.dm.command_access_register.regno = 0x1001;

    self.dmi_data = self.dm.command_access_register.reg;
    self.dmi_op = RISCV_DMI_OP_WRITE;
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_ABSTRACT_CMD, &self.dmi_data, &self.dmi_op));

    for(self.i = 1; self.i < 31; self.i++) {
        self.dmi_data = 0;
        self.dmi_op = RISCV_DMI_OP_READ;
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_DATA0, &self.dmi_data, &self.dmi_op));
        if(self.i > 1) {
            if(self.dmi_op != RISCV_DMI_RESULT_DONE)
            {
                PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
                regs[self.i - 1] = 0xffffffff;
            } else {
                regs[self.i - 1] = self.dmi_data;
            }
        }
    }

    // Clear Abstract Command Autoexec
    self.dm.abstractauto.reg = 0;
    self.dmi_data = self.dm.abstractauto.reg;
    self.dmi_op = RISCV_DMI_OP_WRITE;
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_ABSTRACT_AUTO, &self.dmi_data, &self.dmi_op));
    if(self.dmi_op != RISCV_DMI_RESULT_DONE)
    {
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
        regs[30] = 0xffffffff;
    } else {
        regs[30] = self.dmi_data;
    }

    self.dmi_data = 0;
    self.dmi_op = RISCV_DMI_OP_READ;
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_DATA0, &self.dmi_data, &self.dmi_op));

#else // RVL_TARGET_CONFIG_HAS_ABS_CMD_AUTO
    for(self.i = 1; self.i < 32; self.i++) {
        self.dm.command_access_register.reg = 0;
        self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
        self.dm.command_access_register.aarsize = 2;
        self.dm.command_access_register.transfer = 1;
        self.dm.command_access_register.regno = self.i + 0x1000;

        self.dmi_data = self.dm.command_access_register.reg;
        self.dmi_op = RISCV_DMI_OP_WRITE;
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_ABSTRACT_CMD, &self.dmi_data, &self.dmi_op));
        if(self.i > 1) {
            if(self.dmi_op != RISCV_DMI_RESULT_DONE)
            {
                PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
                regs[self.i - 1] = 0xffffffff;
            } else {
                regs[self.i - 1] = self.dmi_data;
            }
        }

        self.dmi_data = 0;
        self.dmi_op = RISCV_DMI_OP_READ;
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_DATA0, &self.dmi_data, &self.dmi_op));
    }
#endif // RVL_TARGET_CONFIG_HAS_ABS_CMD_AUTO

    self.dm.command_access_register.reg = 0;
    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = CSR_DPC;

    self.dmi_data = self.dm.command_access_register.reg;
    self.dmi_op = RISCV_DMI_OP_WRITE;
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(RISCV_DM_ABSTRACT_CMD, &self.dmi_data, &self.dmi_op));
    if(self.dmi_op != RISCV_DMI_RESULT_DONE)
    {
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
        regs[31] = 0xffffffff;
    } else {
        regs[31] = self.dmi_data;
    }

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        regs[32] = 0xffffffff;
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        regs[32] = self.dm.data[0];
    }


#else // optimize
    for(self.i = 1; self.i < 32; self.i++) {
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&regs[self.i], self.i + 0x1000));
    }
    PT_WAIT_THREAD(&self.pt, riscv_read_register(&regs[32], CSR_DPC));
#endif // optimize

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_write_core_registers(const rvl_target_reg_t *regs))
{
    PT_BEGIN(&self.pt);

    for(self.i = 1; self.i < 32; self.i++) {
        PT_WAIT_THREAD(&self.pt, riscv_write_register(regs[self.i], self.i + 0x1000));
    }
    PT_WAIT_THREAD(&self.pt, riscv_write_register(regs[32], CSR_DPC));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_read_register(rvl_target_reg_t *reg, int regno))
{
    PT_BEGIN(&self.pt);

    if(regno <= 31) { // GPRs
        PT_WAIT_THREAD(&self.pt, riscv_read_register(reg, regno + 0x1000));
    } else if(regno == 32) { // PC
        PT_WAIT_THREAD(&self.pt, riscv_read_register(reg, CSR_DPC));
    } else if(regno <= 64) { // FPRs
        PT_WAIT_THREAD(&self.pt, riscv_read_register(reg, regno + 0x1000 - 1));
    } else if(regno <= (0x1000 + 64)) { // CSRs
        PT_WAIT_THREAD(&self.pt, riscv_read_register(reg, regno - 65));
    } else if(regno == 4161) {  // priv
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
        *reg = self.dcsr.prv;
    } else {
        *reg = 0xffffffff;
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_write_register(rvl_target_reg_t reg, int regno))
{
    PT_BEGIN(&self.pt);

    if(regno <= 31) { // GPRs
        PT_WAIT_THREAD(&self.pt, riscv_write_register(reg, regno + 0x1000));
    } else if(regno == 32) { // PC
        PT_WAIT_THREAD(&self.pt, riscv_write_register(reg, CSR_DPC));
    } else if(regno <= 64) { // FPRs
        PT_WAIT_THREAD(&self.pt, riscv_write_register(reg, regno + 0x1000 - 1));
    } else if(regno <= (0x1000 + 64)) { // CSRs
        PT_WAIT_THREAD(&self.pt, riscv_write_register(reg, regno - 65));
    } else if(regno == 4161) {  // priv
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
        self.dcsr.prv= reg;
        PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr.word, CSR_DCSR));
    } else {

    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_read_memory(uint8_t* mem, rvl_target_addr_t addr, size_t len))
{
    PT_BEGIN(&self.pt);

    if(((uint32_t)mem & 3) == 0 && (addr & 3) == 0 && (len & 3) == 0) {
        PT_WAIT_THREAD(&self.pt, riscv_read_mem(mem, addr, len / 4, RISCV_AAMSIZE_32BITS));
    } else if(((uint32_t)mem & 1) == 0 && (addr & 1) == 0 && (len & 1) == 0) {
        PT_WAIT_THREAD(&self.pt, riscv_read_mem(mem, addr, len / 2, RISCV_AAMSIZE_16BITS));
    } else {
        PT_WAIT_THREAD(&self.pt, riscv_read_mem(mem, addr, len, RISCV_AAMSIZE_8BITS));
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_write_memory(const uint8_t* mem, rvl_target_addr_t addr, size_t len))
{
    PT_BEGIN(&self.pt);

    if(((uint32_t)mem & 3) == 0 && (addr & 3) == 0 && (len & 3) == 0) {
        PT_WAIT_THREAD(&self.pt, riscv_write_mem(mem, addr, len / 4, RISCV_AAMSIZE_32BITS));
    } else if(((uint32_t)mem & 1) == 0 && (addr & 1) == 0 && (len & 1) == 0) {
        PT_WAIT_THREAD(&self.pt, riscv_write_mem(mem, addr, len / 2, RISCV_AAMSIZE_16BITS));
    } else {
        PT_WAIT_THREAD(&self.pt, riscv_write_mem(mem, addr, len, RISCV_AAMSIZE_8BITS));
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_reset(void))
{
    PT_BEGIN(&self.pt);

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.dmactive = 1;
    self.dm.dmcontrol.haltreq = 1;
    self.dm.dmcontrol.setresethaltreq = 1;
    self.dm.dmcontrol.hartreset = 1;
    self.dm.dmcontrol.ndmreset = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    rvl_jtag_srst_put(0);

    timer_set(&self.timer, 100 * 1000);
    PT_WAIT_UNTIL(&self.pt, timer_expired(&self.timer));

    rvl_jtag_srst_put(1);

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.dmactive = 1;
    self.dm.dmcontrol.haltreq = 1;
    self.dm.dmcontrol.setresethaltreq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    for(self.i = 0; self.i < 100; self.i++) {
        timer_set(&self.timer, 10 * 1000);
        PT_WAIT_UNTIL(&self.pt, timer_expired(&self.timer));

        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
        if(self.dm.dmstatus.allhalted) {
            break;
        }
    }

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.dmactive = 1;
    self.dm.dmcontrol.haltreq = 1;
    self.dm.dmcontrol.clrresethaltreq = 1;
    if(self.dm.dmstatus.allhavereset) {
        self.dm.dmcontrol.ackhavereset = 1;
    }
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_halt(void))
{
    PT_BEGIN(&self.pt);

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.haltreq = 1;
    self.dm.dmcontrol.dmactive = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_halt_check(rvl_target_halt_info_t* halt_info))
{
    int has_watchpoint;

    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));

    if(self.dm.dmstatus.allhalted) {
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
        if(self.dcsr.cause == RISCV_CSR_DCSR_CAUSE_EBREAK) {
            halt_info->reason = rvl_target_halt_reason_software_breakpoint;
        } else if(self.dcsr.cause == RISCV_CSR_DCSR_CAUSE_TRIGGER) {
            halt_info->reason = rvl_target_halt_reason_other;
#if 0 // GD32VF103 do not implement mcontrol.hit
            for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
                if(self.hardware_breakpoints[self.i].type != rvl_target_breakpoint_type_unused) {
                    self.tselect = self.i;
                    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.tselect, CSR_TSELECT));
                    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.mcontrol.reg, CSR_TDATA1));
                    if(self.mcontrol.hit) {
                        if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_hardware) {
                            halt_info->reason = rvl_target_halt_reason_hardware_breakpoint;
                        } else if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_write_watchpoint) {
                            halt_info->reason = rvl_target_halt_reason_write_watchpoint;
                            halt_info->addr = self.hardware_breakpoints[self.i].addr;
                        } else if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_read_watchpoint) {
                            halt_info->reason = rvl_target_halt_reason_read_watchpoint;
                            halt_info->addr = self.hardware_breakpoints[self.i].addr;
                        } else if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_access_watchpoint) {
                            halt_info->reason = rvl_target_halt_reason_access_watchpoint;
                            halt_info->addr = self.hardware_breakpoints[self.i].addr;
                        } else {
                            halt_info->reason = rvl_target_halt_reason_other;
                        }
                        break;
                    }
                }
            }
#else
            PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dpc, CSR_DPC));
            for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
                if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_hardware) {
                    if(self.hardware_breakpoints[self.i].addr == self.dpc) {
                        halt_info->reason = rvl_target_halt_reason_hardware_breakpoint;
                        break;
                    }
                }
            }

            if(halt_info->reason == rvl_target_halt_reason_other) {
                has_watchpoint = 0;
                for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
                    if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_write_watchpoint) {
                        has_watchpoint = 1;
                        break;
                    }
                    if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_read_watchpoint) {
                        has_watchpoint = 1;
                        break;
                    }
                    if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_access_watchpoint) {
                        has_watchpoint = 1;
                        break;
                    }
                }

                if(has_watchpoint) {
                    PT_WAIT_THREAD(&self.pt, riscv_read_mem((uint8_t*)&self.wp_inst, self.dpc, 4, RISCV_AAMSIZE_16BITS));
                    riscv_parse_watchpoint_inst(self.wp_inst, &self.wp_addr_base_regno, &self.wp_addr_offset);
                    PT_WAIT_THREAD(&self.pt, riscv_read_register(&(halt_info->addr), self.wp_addr_base_regno + 0x1000));
                    halt_info->addr += self.wp_addr_offset;

                    if(halt_info->addr != 0xffffffff) {
                        for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
                            if(halt_info->addr == self.hardware_breakpoints[self.i].addr) {
                                if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_write_watchpoint) {
                                    halt_info->reason = rvl_target_halt_reason_write_watchpoint;
                                }
                                if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_read_watchpoint) {
                                    halt_info->reason = rvl_target_halt_reason_read_watchpoint;
                                }
                                if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_access_watchpoint) {
                                    halt_info->reason = rvl_target_halt_reason_access_watchpoint;
                                }
                                break;
                            }
                        }
                    }
                }
            }
#endif
        } else {
            halt_info->reason = rvl_target_halt_reason_other;
        }
    } else {
        halt_info->reason = rvl_target_halt_reason_running;
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_resume(void))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
    self.dcsr.step = 0;
    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr.word, CSR_DCSR));

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.resumereq = 1;
    self.dm.dmcontrol.dmactive = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_step(void))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr.word, CSR_DCSR));
    self.dcsr.step = 1;
    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr.word, CSR_DCSR));

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.resumereq = 1;
    self.dm.dmcontrol.dmactive = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_insert_breakpoint(rvl_target_breakpoint_type_t type, rvl_target_addr_t addr, int kind, int* err))
{
    const uint16_t c_ebreak = 0x9002;
    const uint32_t ebreak = 0x00100073;

    PT_BEGIN(&self.pt);

    if(type == rvl_target_breakpoint_type_software) {
        for(self.i = 0; self.i < RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM; self.i++) {
            if(self.software_breakpoints[self.i].type == rvl_target_breakpoint_type_unused) {
                self.software_breakpoints[self.i].type = type;
                self.software_breakpoints[self.i].addr = addr;
                self.software_breakpoints[self.i].kind = kind;

                PT_WAIT_THREAD(&self.pt, riscv_read_mem(
                        (uint8_t*)&self.software_breakpoints[self.i].orig_inst,
                        addr, kind == 2 ? 1 : 2, RISCV_AAMSIZE_16BITS));
                PT_WAIT_THREAD(&self.pt, riscv_write_mem(
                        kind == 2 ? (const uint8_t*)&c_ebreak : (const uint8_t*)&ebreak,
                        addr, kind == 2 ? 1 : 2, RISCV_AAMSIZE_16BITS));
                break;
            }
        }

        if(self.i == RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM) {
            *err = 0x0e;
            PT_EXIT(&self.pt);
        }
    } else {
        for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
            if(self.hardware_breakpoints[self.i].type == rvl_target_breakpoint_type_unused) {
                self.hardware_breakpoints[self.i].type = type;
                self.hardware_breakpoints[self.i].addr = addr;
                self.hardware_breakpoints[self.i].kind = kind;
                self.tselect = self.i;

                PT_WAIT_THREAD(&self.pt, riscv_write_register(self.tselect, CSR_TSELECT));

                self.mcontrol.reg = 0;
                self.mcontrol.type = 2;
                self.mcontrol.dmode = 1;
                self.mcontrol.action = 1;
                self.mcontrol.m = 1;
                self.mcontrol.s = 1;
                self.mcontrol.u = 1;

                switch(type) {
                case rvl_target_breakpoint_type_hardware:
                    self.mcontrol.execute = 1;
                    break;
                case rvl_target_breakpoint_type_write_watchpoint:
                    self.mcontrol.store = 1;
                    break;
                case rvl_target_breakpoint_type_read_watchpoint:
                    self.mcontrol.load = 1;
                    break;
                case rvl_target_breakpoint_type_access_watchpoint:
                    self.mcontrol.store = 1;
                    self.mcontrol.load = 1;
                    break;
                default:
                    break;
                }
#if 0
                switch(kind) {
                case 1:
                    self.mcontrol.sizelo = 1;
                    break;
                case 2:
                    self.mcontrol.sizelo = 2;
                    break;
                case 4:
                    self.mcontrol.sizelo = 3;
                    break;
                default:
                    break;
                }
#endif
                PT_WAIT_THREAD(&self.pt, riscv_write_register(self.mcontrol.reg, CSR_TDATA1));
                PT_WAIT_THREAD(&self.pt, riscv_write_register(self.hardware_breakpoints[self.tselect].addr, CSR_TDATA2));
                break;
            }
        }

        if(self.i == RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM) {
            *err = 0x0e;
            PT_EXIT(&self.pt);
        }
    }

    *err = 0;
    PT_END(&self.pt);
}


PT_THREAD(rvl_target_remove_breakpoint(rvl_target_breakpoint_type_t type,rvl_target_addr_t addr, int kind, int* err))
{
    PT_BEGIN(&self.pt);

    if(type == rvl_target_breakpoint_type_software) {
        for(self.i = 0; self.i < RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM; self.i++) {
            if(self.software_breakpoints[self.i].type == type
                    && self.software_breakpoints[self.i].addr == addr
                    && self.software_breakpoints[self.i].kind == kind) {
                PT_WAIT_THREAD(&self.pt, riscv_write_mem(
                        (const uint8_t*)&self.software_breakpoints[self.i].orig_inst,
                        addr, kind == 2 ? 1 : 2, RISCV_AAMSIZE_16BITS));

                self.software_breakpoints[self.i].type = rvl_target_breakpoint_type_unused;
                break;
            }
        }

        if(self.i == RVL_TARGET_CONFIG_SOFTWARE_BREAKPOINT_NUM) {
            *err = 0x0e;
            PT_EXIT(&self.pt);
        }
    } else {
        for(self.i = 0; self.i < RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM; self.i++) {
            if(self.hardware_breakpoints[self.i].type == type
                    && self.hardware_breakpoints[self.i].addr == addr
                    && self.hardware_breakpoints[self.i].kind == kind) {
                self.tselect = self.i;

                PT_WAIT_THREAD(&self.pt, riscv_write_register(self.tselect, CSR_TSELECT));
                PT_WAIT_THREAD(&self.pt, riscv_write_register(0, CSR_TDATA1));

                self.hardware_breakpoints[self.i].type = rvl_target_breakpoint_type_unused;
                break;
            }
        }

        if(self.i == RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM) {
            *err = 0x0e;
            PT_EXIT(&self.pt);
        }
    }

    *err = 0;
    PT_END(&self.pt);
}


static PT_THREAD(riscv_read_register(rvl_target_reg_t* reg, uint32_t regno))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.command_access_register.reg = 0;
    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = regno;
    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        *reg = 0xffffffff;
    } else {
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        *reg = self.dm.data[0];
    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_write_register(rvl_target_reg_t reg, uint32_t regno))
{
    PT_BEGIN(&self.pt_sub);

    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_DATA0, reg, &self.dmi_result));

    self.dm.command_access_register.reg = 0;
    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.write = 1;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = regno;
    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {

    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_read_mem(uint8_t* mem, rvl_target_addr_t addr, size_t len, int aamsize))
{
    uint8_t* pbyte;
    uint16_t* phalfword;
    uint32_t* pword;

    PT_BEGIN(&self.pt_sub);

#if 1 // optimize
    for(self.ii = 0; self.ii < len + 1; self.ii++) {
        self.dmi_data = addr + (self.ii << aamsize);
        self.dmi_op = RISCV_DMI_OP_WRITE;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dtm_dmi(RISCV_DM_DATA1, &self.dmi_data, &self.dmi_op));
        if(self.ii > 0) {
            switch(aamsize) {
            case 0:
                pbyte = (uint8_t*)mem;
                pbyte[self.ii - 1] = self.dmi_data & 0xff;
                break;
            case 1:
                phalfword = (uint16_t*)mem;
                phalfword[self.ii - 1] = self.dmi_data & 0xffff;
                break;
            case 2:
                pword = (uint32_t*)mem;
                pword[self.ii - 1] = self.dmi_data;
                break;
            default:
                break;
            }
        }

        if(self.ii == len) {
            break;
        }

        self.dm.command_access_memory.reg = 0;
        self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
        self.dm.command_access_memory.aamsize = aamsize;

        self.dmi_data = self.dm.command_access_memory.reg;
        self.dmi_op = RISCV_DMI_OP_WRITE;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dtm_dmi(RISCV_DM_ABSTRACT_CMD, &self.dmi_data, &self.dmi_op));

        self.dmi_data = 0;
        self.dmi_op = RISCV_DMI_OP_READ;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dtm_dmi(RISCV_DM_DATA0, &self.dmi_data, &self.dmi_op));
    }

    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }

#else

    for(self.ii = 0; self.ii < len; self.ii++) {
        self.dm.data[1] = addr + (self.ii << aamsize);
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));

        self.dm.command_access_memory.reg = 0;
        self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
        self.dm.command_access_memory.aamsize = aamsize;

        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
        if(self.dm.abstractcs.cmderr) {
            rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

            self.dm.abstractcs.reg = 0;
            self.dm.abstractcs.cmderr = 0x7;
            PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
            self.dm.data[0] = 0xffffffff;
        } else {
            PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        }

        switch(aamsize) {
        case 0:
            pbyte = (uint8_t*)mem;
            pbyte[self.ii] = self.dm.data[0] & 0xff;
            break;
        case 1:
            phalfword = (uint16_t*)mem;
            phalfword[self.ii] = self.dm.data[0] & 0xffff;
            break;
        case 2:
            pword = (uint32_t*)mem;
            pword[self.ii] = self.dm.data[0];
            break;
        default:
            break;
        }
    }
#endif

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_write_mem(const uint8_t* mem, rvl_target_addr_t addr, size_t len, int aamsize))
{
    const uint8_t* pbyte;
    const uint16_t* phalfword;
    const uint32_t* pword;

    PT_BEGIN(&self.pt_sub);

#if 1 // optimize
    for(self.ii = 0; self.ii < len; self.ii++) {
        self.dmi_data = addr + (self.ii << aamsize);
        self.dmi_op = RISCV_DMI_OP_WRITE;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dtm_dmi(RISCV_DM_DATA1, &self.dmi_data, &self.dmi_op));

        switch(aamsize) {
        case RISCV_AAMSIZE_8BITS:
            pbyte = (const uint8_t*)mem;
            self.dm.data[0] = pbyte[self.ii];
            break;
        case RISCV_AAMSIZE_16BITS:
            phalfword = (const uint16_t*)mem;
            self.dm.data[0] = phalfword[self.ii];
            break;
        case RISCV_AAMSIZE_32BITS:
            pword = (const uint32_t*)mem;
            self.dm.data[0] = pword[self.ii];
            break;
        default:
            break;
        }

        self.dmi_data = self.dm.data[0];
        self.dmi_op = RISCV_DMI_OP_WRITE;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dtm_dmi(RISCV_DM_DATA0, &self.dmi_data, &self.dmi_op));

        self.dm.command_access_memory.reg = 0;
        self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
        self.dm.command_access_memory.aamsize = aamsize;
        self.dm.command_access_memory.write = 1;

        self.dmi_data = self.dm.command_access_memory.reg;
        self.dmi_op = RISCV_DMI_OP_WRITE;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dtm_dmi(RISCV_DM_ABSTRACT_CMD, &self.dmi_data, &self.dmi_op));
    }

    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }
#else
    for(self.ii = 0; self.ii < len; self.ii++) {
        self.dm.data[1] = addr + (self.ii << aamsize);
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));

        switch(aamsize) {
        case RISCV_AAMSIZE_8BITS:
            pbyte = (const uint8_t*)mem;
            self.dm.data[0] = pbyte[self.ii];
            break;
        case RISCV_AAMSIZE_16BITS:
            phalfword = (const uint16_t*)mem;
            self.dm.data[0] = phalfword[self.ii];
            break;
        case RISCV_AAMSIZE_32BITS:
            pword = (const uint32_t*)mem;
            self.dm.data[0] = pword[self.ii];
            break;
        default:
            break;
        }
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_DATA0, (rvl_dmi_reg_t)(self.dm.data[0]), &self.dmi_result));

        self.dm.command_access_memory.reg = 0;
        self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
        self.dm.command_access_memory.aamsize = aamsize;
        self.dm.command_access_memory.write = 1;

        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
        if(self.dm.abstractcs.cmderr) {
            rvl_target_set_error(riscv_abstractcs_cmderr_str[self.dm.abstractcs.cmderr]);

            self.dm.abstractcs.reg = 0;
            self.dm.abstractcs.cmderr = 0x7;
            PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        }
    }
#endif

    PT_END(&self.pt_sub);
}


static void riscv_parse_watchpoint_inst(uint32_t inst, uint32_t* regno, uint32_t* offset)
{
    uint32_t opcode;
    int32_t offset_s;
    if((inst & 0x3) == 0x3) {
        // 32 bits Instruction
        opcode = inst & 0x7f;
        switch(opcode) {
        case 0x03: // LOAD
        case 0x07: // LOAD-FP
            *regno = (inst >> 15) & 0x1f;
            offset_s = inst & 0xfff00000;
            offset_s = offset_s >> 20;
            break;
        case 0x23: // STORE
        case 0x27: // STORE-FP
            *regno = (inst >> 15) & 0x1f;
            offset_s = (inst & 0xfe000000) | ((inst << 13) & 0x01f00000);
            offset_s = offset_s >> 20;
            break;
        default:
            *regno = 0;
            offset_s = 0;
            break;
        }
    } else {
        // 16 bits Instruction, RV32 和 RV64 是不一样的，当前仅考虑 RV32
        opcode = ((inst & 0xe000) >> 11) | (inst & 0x3);
        switch(opcode) {
        case 0x4: // FLD
        case 0x8: // LW
        case 0xc: // FLW, LD
        case 0x14: // FSD, SQ
        case 0x18: // SW
        case 0x1c: // FSW, SD
            *regno = ((inst >> 7) & 0x7) + 8;
            break;

        case 0x6: // FLDSP, LQSP
        case 0xa: // LWSP
        case 0xe: // FLWSP, LDSP
        case 0x16: // FSDSP, SQSP
        case 0x1a: // SWSP
        case 0x1e: // FSWSP, SDSP
            *regno = 2;
            break;

        default:
            *regno = 0;
            break;
        }

        switch(opcode) {
        case 0x4: // FLD
        case 0x14: // FSD
            offset_s = ((inst & 0x1c00) >> 7) | ((inst & 0x60) << 1);
            break;

        case 0x8: // LW
        case 0xc: // FLW
        case 0x18: // SW
        case 0x1c: // FSW
            offset_s = ((inst & 0x1c00) >> 7) | ((inst & 0x40) >> 4) | ((inst & 0x20) << 1);
            break;

        case 0x6: // FLDSP
            offset_s = ((inst & 0x1000) >> 7) | ((inst & 0x60) >> 2) | ((inst & 0x1c) << 4);
            break;

        case 0xa: // LWSP
        case 0xe: // FLWSP
            offset_s = ((inst & 0x1000) >> 7) | ((inst & 0x70) >> 2) | ((inst & 0xc) << 4);
            break;

        case 0x16: // FSDSP
            offset_s = ((inst & 0x1c00) >> 7) | ((inst & 0x380) >> 1);
            break;

        case 0x1a: // SWSP
        case 0x1e: // FSWSP
            offset_s = ((inst & 0x1e00) >> 7) | ((inst & 0x180) >> 1);
            break;

        default:
            offset_s = 0;
            break;
        }
    }

    *offset = offset_s;
}


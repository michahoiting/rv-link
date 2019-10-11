#include "rvl-target-config.h"
#include "dm.h"
#include "dmi.h"
#include "encoding.h"
#include "rvl-target.h"
#include "rvl-jtag.h"

#ifndef RVL_TARGET_CONFIG_BREAKPOINT_NUM
#define RVL_TARGET_CONFIG_BREAKPOINT_NUM     4
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


typedef struct riscv_breakpoint_s
{
    rvl_target_breakpoint_type_t type;
    rvl_target_addr_t addr;
    int kind;
}riscv_breakpoint_t;

typedef struct riscv_target_s
{
    struct pt pt;
    struct pt pt_sub;
    riscv_dm_t dm;
    uint32_t dmi_result;
    uint32_t i;
    rvl_target_reg_t dcsr;
    rvl_dtm_idcode_t idcode;
    rvl_dtm_dtmcs_t dtmcs;
    rvl_target_reg_t tselect;
    riscv_csr_mcontrol_t mcontrol;
    riscv_csr_mcontrol_t mcontrol_rb;
    rvl_dmi_reg_t dmi_data;
    uint32_t dmi_op;
    riscv_breakpoint_t breakpoints[RVL_TARGET_CONFIG_BREAKPOINT_NUM];
}riscv_target_t;

static riscv_target_t riscv_target_i;
#define self riscv_target_i


static PT_THREAD(riscv_read_register(rvl_target_reg_t* reg, uint32_t regno));
static PT_THREAD(riscv_write_register(rvl_target_reg_t reg, uint32_t regno));
static PT_THREAD(riscv_read_mem_byte(uint8_t* mem, rvl_target_addr_t addr));
static PT_THREAD(riscv_read_mem_halfword(uint16_t* mem, rvl_target_addr_t addr));
static PT_THREAD(riscv_read_mem_word(uint32_t* mem, rvl_target_addr_t addr));
static PT_THREAD(riscv_write_mem_byte(uint8_t mem, rvl_target_addr_t addr));
static PT_THREAD(riscv_write_mem_halfword(uint16_t mem, rvl_target_addr_t addr));
static PT_THREAD(riscv_write_mem_word(uint32_t mem, rvl_target_addr_t addr));


void riscv_target_init(void)
{
    PT_INIT(&self.pt);
    PT_INIT(&self.pt_sub);

    for(self.i = 0; self.i < RVL_TARGET_CONFIG_BREAKPOINT_NUM; self.i++) {
        self.breakpoints[self.i].type = unused_breakpoint;
    }

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


PT_THREAD(riscv_target_fini_pre(void))
{
    PT_BEGIN(&self.pt);

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13

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

    PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        regs[32] = 0xffffffff;
    } else {
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
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
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr, CSR_DCSR));
        *reg = self.dcsr & 0x3;
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
    } else {

    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_read_memory(uint8_t* mem, rvl_target_addr_t addr, size_t len))
{
    PT_BEGIN(&self.pt);

    if(((uint32_t)mem & 3) == 0 && (addr & 3) == 0 && (len & 3) == 0) {
        for(self.i = 0; self.i < len; self.i += 4) {
            PT_WAIT_THREAD(&self.pt, riscv_read_mem_word((uint32_t*)&mem[self.i], addr + self.i));
        }
    } else if(((uint32_t)mem & 1) == 0 && (addr & 1) == 0 && (len & 1) == 0) {
        for(self.i = 0; self.i < len; self.i += 2) {
            PT_WAIT_THREAD(&self.pt, riscv_read_mem_halfword((uint16_t*)&mem[self.i], addr + self.i));
        }
    } else {
        for(self.i = 0; self.i < len; self.i++) {
            PT_WAIT_THREAD(&self.pt, riscv_read_mem_byte(&mem[self.i], addr + self.i));
        }
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_write_memory(const uint8_t* mem, rvl_target_addr_t addr, size_t len))
{
    PT_BEGIN(&self.pt);

    if(((uint32_t)mem & 3) == 0 && (addr & 3) == 0 && (len & 3) == 0) {
        for(self.i = 0; self.i < len; self.i += 4) {
            PT_WAIT_THREAD(&self.pt, riscv_write_mem_word(*((uint32_t*)&mem[self.i]), addr + self.i));
        }
    } else if(((uint32_t)mem & 1) == 0 && (addr & 1) == 0 && (len & 1) == 0) {
        for(self.i = 0; self.i < len; self.i += 2) {
            PT_WAIT_THREAD(&self.pt, riscv_write_mem_halfword(*((uint16_t*)&mem[self.i]), addr + self.i));
        }
    } else {
        for(self.i = 0; self.i < len; self.i++) {
            PT_WAIT_THREAD(&self.pt, riscv_write_mem_byte(mem[self.i], addr + self.i));
        }
    }

    for(self.i = 0; self.i < len; self.i++) {

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
    self.dm.dmcontrol.ndmreset = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    rvl_jtag_srst_put(0);
    rvl_jtag_delay(100 * 1000); // 100 ms TODO
    rvl_jtag_srst_put(1);

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.dmactive = 1;
    self.dm.dmcontrol.haltreq = 1;
    self.dm.dmcontrol.setresethaltreq = 1;
    self.dm.dmcontrol.ndmreset = 0;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    for(self.i = 0; self.i < 100; self.i++) {
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

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));

    self.dm.dmcontrol.resumereq = 0;
    self.dm.dmcontrol.haltreq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_halt_check(int* halted))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));

    if(self.dm.dmstatus.allhalted) {
        *halted = 1;
    } else {
        *halted = 0;
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_resume(void))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr, CSR_DCSR));
    self.dcsr &= ~(1UL << 2); // step
    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr, CSR_DCSR));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));
    self.dm.dmcontrol.haltreq = 0;
    self.dm.dmcontrol.resumereq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_step(void))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr, CSR_DCSR));
    self.dcsr |= 1 << 2; // step
    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.dcsr, CSR_DCSR));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));
    self.dm.dmcontrol.haltreq = 0;
    self.dm.dmcontrol.resumereq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_insert_breakpoint(rvl_target_breakpoint_type_t type, rvl_target_addr_t addr, int kind, int* err))
{
    PT_BEGIN(&self.pt);

    for(self.i = 0; self.i < RVL_TARGET_CONFIG_BREAKPOINT_NUM; self.i++) {
        if(self.breakpoints[self.i].type == unused_breakpoint) {
            self.breakpoints[self.i].type = type;
            self.breakpoints[self.i].addr = addr;
            self.breakpoints[self.i].kind = kind;
            self.tselect = self.i;

            PT_WAIT_THREAD(&self.pt, riscv_write_register(self.tselect, CSR_TSELECT));
            PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.mcontrol.reg, CSR_TDATA1));

            self.mcontrol.dmode = 1;
            self.mcontrol.action = 1;
            self.mcontrol.m = 1;
            self.mcontrol.u = 1;

            switch(type) {
            case hardware_breakpoint:
                self.mcontrol.execute = 1;
                break;
            case write_watchpoint:
                self.mcontrol.store = 1;
                break;
            case read_watchpoint:
                self.mcontrol.load = 1;
                break;
            case access_watchpoint:
                self.mcontrol.store = 1;
                self.mcontrol.load = 1;
                break;
            default:
                break; // FIXME
            }

//            switch(kind) {
//            case 1:
//                self.mcontrol.sizelo = 1;
//                break;
//            case 2:
//                self.mcontrol.sizelo = 2;
//                break;
//            case 4:
//                self.mcontrol.sizelo = 3;
//                break;
//            default:
//                break; // FIXME
//            }

            PT_WAIT_THREAD(&self.pt, riscv_write_register(self.mcontrol.reg, CSR_TDATA1));
            PT_WAIT_THREAD(&self.pt, riscv_write_register(self.breakpoints[self.tselect].addr, CSR_TDATA2));
            PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.mcontrol_rb.reg, CSR_TDATA1));
            break;
        }
    }

    if(self.i == RVL_TARGET_CONFIG_BREAKPOINT_NUM) {
        *err = 0x0e;
        PT_EXIT(&self.pt);
    }

    *err = 0;
    PT_END(&self.pt);
}


PT_THREAD(rvl_target_remove_breakpoint(rvl_target_breakpoint_type_t type,rvl_target_addr_t addr, int kind, int* err))
{
    PT_BEGIN(&self.pt);

    for(self.i = 0; self.i < RVL_TARGET_CONFIG_BREAKPOINT_NUM; self.i++) {
        if(self.breakpoints[self.i].type == type
                && self.breakpoints[self.i].addr == addr
                && self.breakpoints[self.i].kind == kind) {
            self.tselect = self.i;
            break;
        }
    }

    if(self.i == RVL_TARGET_CONFIG_BREAKPOINT_NUM) {
        *err = 0x0e;
        PT_EXIT(&self.pt);
    }

    PT_WAIT_THREAD(&self.pt, riscv_write_register(self.tselect, CSR_TSELECT));
    PT_WAIT_THREAD(&self.pt, riscv_write_register(0, CSR_TDATA1));

    self.breakpoints[self.i].type = unused_breakpoint;
    self.breakpoints[self.i].addr = 0;
    self.breakpoints[self.i].kind = 0;
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
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt_sub, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {

    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_read_mem_byte(uint8_t* mem, rvl_target_addr_t addr))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.data[1] = addr;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 0; // Access the lowest 8 bits of the memory location.

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        *mem = 0xff;
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        *mem = self.dm.data[0] & 0xff;
    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_read_mem_halfword(uint16_t* mem, rvl_target_addr_t addr))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.data[1] = addr;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 1; // Access the lowest 16 bits of the memory location.

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        *mem = 0xffff;
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        *mem = self.dm.data[0] & 0xffff;
    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_read_mem_word(uint32_t* mem, rvl_target_addr_t addr))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.data[1] = addr;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 2; // Access the lowest 32 bits of the memory location.

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
        *mem = 0xffffffff;
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        *mem = self.dm.data[0] & 0xffffffff;
    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_write_mem_byte(uint8_t mem, rvl_target_addr_t addr))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.data[1] = addr;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));
    self.dm.data[0] = mem;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA0, (rvl_dmi_reg_t)(self.dm.data[0]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 0; // Access the lowest 8 bits of the memory location.
    self.dm.command_access_memory.write = 1;

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_write_mem_halfword(uint16_t mem, rvl_target_addr_t addr))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.data[1] = addr;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));
    self.dm.data[0] = mem;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA0, (rvl_dmi_reg_t)(self.dm.data[0]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 1; // Access the lowest 16 bits of the memory location.
    self.dm.command_access_memory.write = 1;

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }

    PT_END(&self.pt_sub);
}


static PT_THREAD(riscv_write_mem_word(uint32_t mem, rvl_target_addr_t addr))
{
    PT_BEGIN(&self.pt_sub);

    self.dm.data[1] = addr;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));
    self.dm.data[0] = mem;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA0, (rvl_dmi_reg_t)(self.dm.data[0]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 2; // Access the lowest 32 bits of the memory location.
    self.dm.command_access_memory.write = 1;

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_memory.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }

    PT_END(&self.pt_sub);
}



#include "dm.h"
#include "dmi.h"
#include "encoding.h"
#include "rvl-target.h"

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
}riscv_target_t;

static riscv_target_t riscv_target_i;
#define self riscv_target_i


static PT_THREAD(riscv_read_register(rvl_target_reg_t* reg, uint32_t regno));
static PT_THREAD(riscv_write_register(rvl_target_reg_t reg, uint32_t regno));
static PT_THREAD(riscv_read_mem_byte(uint8_t* mem, rvl_target_addr_t addr));


void rvl_target_init(void)
{
    PT_INIT(&self.pt);
    PT_INIT(&self.pt_sub);

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13

    rvl_dmi_init();

    while(rvl_dtm_idcode(&self.idcode) < PT_EXITED) {}
    while(rvl_dtm_dtmcs(&self.dtmcs) < PT_EXITED) {}

    self.dm.dmcontrol.reg = 0;
    self.dm.dmcontrol.dmactive = 1;
    while(rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result) < PT_EXITED) {}

#else
#error
#endif
}


void rvl_target_fini(void)
{
#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13

    self.dm.dmcontrol.reg = 0;
    while(rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result) < PT_EXITED) {}

    rvl_dmi_fini();

#else
#error
#endif
}


PT_THREAD(rvl_target_read_core_registers(rvl_target_reg_t *regs))
{
    PT_BEGIN(&self.pt);

    regs[0] = 0x0;
    for(self.i = 1; self.i < 32; self.i++) {
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&regs[self.i], self.i + 0x1000));
    }
    PT_WAIT_THREAD(&self.pt, riscv_read_register(&regs[32], CSR_DPC));

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
    } else if(regno == 4146) {  // priv
        PT_WAIT_THREAD(&self.pt, riscv_read_register(&self.dcsr, CSR_DCSR));
        *reg = self.dcsr & 0x3;
    } else {
        *reg = 0xffffffff;
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_read_memory(uint8_t* mem, rvl_target_addr_t addr, size_t len))
{
    PT_BEGIN(&self.pt);

    for(self.i = 0; self.i < len; self.i++) {
        PT_WAIT_THREAD(&self.pt, riscv_read_mem_byte(&mem[self.i], addr + self.i));
    }

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

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_remove_breakpoint(rvl_target_breakpoint_type_t type,rvl_target_addr_t addr, int kind, int* err))
{
    PT_BEGIN(&self.pt);

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

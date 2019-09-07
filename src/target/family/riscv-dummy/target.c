#include "interface/rvl-target.h"

typedef struct gdb_cmd_riscv_dummy_s
{
    struct pt pt;
    uint32_t i;
    uint8_t *p;
    uint32_t addr;
}gdb_cmd_riscv_dummt_t;

static gdb_cmd_riscv_dummt_t gdb_cmd_riscv_dummt_i;
#define self gdb_cmd_riscv_dummt_i


void rvl_target_init(void)
{
    PT_INIT(&self.pt);
}


PT_THREAD(rvl_target_read_registers(rvl_target_reg_t *regs))
{
    PT_BEGIN(&self.pt);

    for(self.i = 0; self.i < 32; self.i++) {
        regs[self.i] = self.i;
        PT_YIELD(&self.pt);
    }
    regs[self.i] = (uint32_t)&rvl_target_read_registers;
    PT_YIELD(&self.pt);

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_read_memory(uint8_t* mem, rvl_target_addr_t addr, size_t len))
{
    PT_BEGIN(&self.pt);

    self.p = mem;
    self.addr = addr;
    for(self.i = 0; self.i < len; self.i++) {
        *self.p = *((uint8_t*)self.addr);
        self.p++;
        self.addr++;
        PT_YIELD(&self.pt);
    }

    PT_END(&self.pt);
}

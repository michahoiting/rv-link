#include "dtm.h"
#include "rvl-tap.h"
#include "rvl-assert.h"

typedef struct rvl_dtm_s {
    struct pt pt;
    uint8_t abits;
    uint32_t in[2];
    uint32_t out[2];
    uint8_t last_jtag_reg;
    uint8_t idle;
}rvl_dtm_t;

static rvl_dtm_t rvl_dtm_i;
#define self rvl_dtm_i


void rvl_dtm_init(void)
{
    PT_INIT(&self.pt);
    self.last_jtag_reg = 0;

    rvl_tap_init();
    rvl_tap_go_idle();
}


PT_THREAD(rvl_dtm_idcode(rvl_dtm_idcode_t* idcode))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_IDCODE) {
        self.in[0] = RISCV_DTM_JTAG_REG_IDCODE;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_IDCODE;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = 0;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    idcode->word = self.out[0];

    PT_END(&self.pt);
}


PT_THREAD(rvl_dtm_dtmcs_dmireset(void))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = RISCV_DTMCS_DMI_RESET;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    if(self.idle) {
        rvl_tap_run(self.idle);
        PT_YIELD(&self.pt);
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_dtm_run(uint32_t ticks))
{
    PT_BEGIN(&self.pt);

    rvl_tap_run(ticks);
    PT_YIELD(&self.pt);

    PT_END(&self.pt);
}



#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
PT_THREAD(rvl_dtm_dtmcs(rvl_dtm_dtmcs_t* dtmcs))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = 0;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    dtmcs->word = self.out[0];

    self.abits = dtmcs->abits;
    self.idle = dtmcs->idle;

    if(self.idle > 0) {
        self.idle -= 1;
    }

    PT_END(&self.pt);
}
#elif RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P11
PT_THREAD(rvl_dtm_dtmcs(rvl_dtm_dtmcs_t* dtmcs))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = 0;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    dtmcs->word = self.out[0];

    self.abits = dtmcs->loabits | (dtmcs->hiabits << 4);
    self.idle = dtmcs->idle;
    if(self.idle > 0) {
        self.idle -= 1;
    }

    PT_END(&self.pt);
}
#else
#error
#endif

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
PT_THREAD(rvl_dtm_dtmcs_dmihardreset(void))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = RISCV_DTMCS_DMI_HARD_RESET;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    if(self.idle) {
        rvl_tap_run(self.idle);
        PT_YIELD(&self.pt);
    }

    PT_END(&self.pt);
}
#endif


#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
PT_THREAD(rvl_dtm_dmi(uint32_t addr, uint32_t* data, uint32_t* op))
{
    PT_BEGIN(&self.pt);

    rvl_assert(addr <= ((1 << self.abits) - 1));
    rvl_assert(data != NULL);
    rvl_assert(op != NULL && *op <= 3);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DMI) {
        self.in[0] = RISCV_DTM_JTAG_REG_DMI;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DMI;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = (*data << 2) | (*op & 0x3);
    self.in[1] = (*data >> 30) | (addr << 2);
    rvl_tap_shift_dr(self.out, self.in, 32 + 2 + self.abits);
    PT_YIELD(&self.pt);

    if(self.idle) {
        rvl_tap_run(self.idle);
        PT_YIELD(&self.pt);
    }

    *op = self.out[0] & 0x3;
    *data = (self.out[0] >> 2) | (self.out[1] << 30);

    PT_END(&self.pt);
}
#elif RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P11
PT_THREAD(rvl_dtm_dmi(uint32_t addr, uint32_t* data, uint32_t* op))
{
    PT_BEGIN(&self.pt);

    rvl_assert(addr <= ((1 << self.abits) - 1));
    rvl_assert(data != NULL);
    rvl_assert(op != NULL && *op <= 3);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DMI) {
        self.in[0] = RISCV_DTM_JTAG_REG_DMI;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DMI;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = (*data << 2) | (*op & 0x3);
    self.in[1] = (*data >> 30) | (addr << 4);
    rvl_tap_shift_dr(self.out, self.in, 34 + 2 + self.abits);
    PT_YIELD(&self.pt);

    if(self.idle) {
        rvl_tap_run(self.idle);
        PT_YIELD(&self.pt);
    }

    *op = self.out[0] & 0x3;
    *data = (self.out[0] >> 2) | (self.out[1] << 30);

    PT_END(&self.pt);
}
#else
#error
#endif


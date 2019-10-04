#include "rvl-target-config.h"
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


#ifdef RVL_TARGET_CONFIG_DTM_YIELD_EN
#define RVL_DTM_YIELD()     PT_YIELD(&self.pt)
#else
#define RVL_DTM_YIELD()
#endif


void rvl_dtm_init(void)
{
    PT_INIT(&self.pt);
    self.last_jtag_reg = 0;

    rvl_tap_init();
    rvl_tap_go_idle();
}


void rvl_dtm_fini(void)
{
    rvl_tap_fini();
}


PT_THREAD(rvl_dtm_idcode(rvl_dtm_idcode_t* idcode))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_IDCODE) {
        self.in[0] = RISCV_DTM_JTAG_REG_IDCODE;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_IDCODE;
        rvl_tap_shift_ir(self.out, self.in, 5);
        RVL_DTM_YIELD();
    }

    self.in[0] = 0;
    rvl_tap_shift_dr(self.out, self.in, 32);
    RVL_DTM_YIELD();

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
        RVL_DTM_YIELD();
    }

    self.in[0] = RISCV_DTMCS_DMI_RESET;
    rvl_tap_shift_dr(self.out, self.in, 32);
    RVL_DTM_YIELD();

    if(self.idle) {
        rvl_tap_run(self.idle);
        RVL_DTM_YIELD();
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_dtm_run(uint32_t ticks))
{
    PT_BEGIN(&self.pt);

    rvl_tap_run(ticks);
    RVL_DTM_YIELD();

    PT_END(&self.pt);
}


PT_THREAD(rvl_dtm_dtmcs(rvl_dtm_dtmcs_t* dtmcs))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        RVL_DTM_YIELD();
    }

    self.in[0] = 0;
    rvl_tap_shift_dr(self.out, self.in, 32);
    RVL_DTM_YIELD();

    dtmcs->word = self.out[0];

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
    self.abits = dtmcs->abits;
#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
    self.abits = dtmcs->loabits | (dtmcs->hiabits << 4);
#else
#error
#endif

    self.idle = dtmcs->idle;

    if(self.idle > 0) {
        self.idle -= 1;
    }

    PT_END(&self.pt);
}


#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
PT_THREAD(rvl_dtm_dtmcs_dmihardreset(void))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        RVL_DTM_YIELD();
    }

    self.in[0] = RISCV_DTMCS_DMI_HARD_RESET;
    rvl_tap_shift_dr(self.out, self.in, 32);
    RVL_DTM_YIELD();

    if(self.idle) {
        rvl_tap_run(self.idle);
        RVL_DTM_YIELD();
    }

    PT_END(&self.pt);
}
#endif



PT_THREAD(rvl_dtm_dmi(uint32_t addr, rvl_dmi_reg_t* data, uint32_t* op))
{
    PT_BEGIN(&self.pt);

    rvl_assert(addr <= ((1 << self.abits) - 1));
    rvl_assert(data != NULL);
    rvl_assert(op != NULL && *op <= 3);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DMI) {
        self.in[0] = RISCV_DTM_JTAG_REG_DMI;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DMI;
        rvl_tap_shift_ir(self.out, self.in, 5);
        RVL_DTM_YIELD();
    }

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
    self.in[0] = (*data << 2) | (*op & 0x3);
    self.in[1] = (*data >> 30) | (addr << 2);
    rvl_tap_shift_dr(self.out, self.in, 32 + 2 + self.abits);
#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
    self.in[0] = (*data << 2) | (*op & 0x3);
    self.in[1] = ((*data >> 30) & 0xf) | (addr << 4);
    rvl_tap_shift_dr(self.out, self.in, 34 + 2 + self.abits);
#else
#error
#endif
    RVL_DTM_YIELD();

    if(self.idle) {
        rvl_tap_run(self.idle);
        RVL_DTM_YIELD();
    }

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
    *op = self.out[0] & 0x3;
    *data = (self.out[0] >> 2) | (self.out[1] << 30);
#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
    *op = self.out[0] & 0x3;
    *data = (self.out[0] >> 2) | ((self.out[1] & 0xf) << 30);
#else
#error
#endif

    PT_END(&self.pt);
}


uint32_t rvl_dtm_get_dtmcs_abits(void)
{
    return (uint32_t)self.abits;
}


uint32_t rvl_dtm_get_dtmcs_idle(void)
{
    return (uint32_t)self.idle;
}

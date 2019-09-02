#include "pt.h"
#include "rvl_jtag.h"
#include "rvl_tap.h"


typedef struct task_jtag_s
{
    struct pt pt;

    uint32_t old_reg[1];
    uint32_t new_reg[1];

    uint32_t idcode[1];
    uint32_t idcode_version;
    uint32_t idcode_part_number;
    uint32_t idcode_manufid;
    uint32_t idcode_lsb;

    uint32_t dtmcs[1];
    uint32_t dtmcs_version;
    uint32_t dtmcs_abits;
    uint32_t dtmcs_dmistat;
    uint32_t dtmcs_idle;
}task_jtag_t;

static task_jtag_t task_jtag_i;

#define self task_jtag_i

void task_jtag_init(void)
{
    PT_INIT(&self.pt);
}

PT_THREAD(task_jtag_poll(void))
{
    PT_BEGIN(&self.pt);

    rvl_jtag_init();
    PT_YIELD(&self.pt);

    rvl_tap_go_idle();
    PT_YIELD(&self.pt);

    // idcode, FE310-G000's idcode: 0x10e31913 (mfg: 0x489 (SiFive, Inc.), part: 0x0e31, ver: 0x1)
    self.new_reg[0] = 0x01;
    rvl_tap_shift_ir(self.old_reg, self.new_reg, 5);
    PT_YIELD(&self.pt);

    self.new_reg[0] = 0x00;
    rvl_tap_shift_dr(self.idcode, self.new_reg, 32);
    PT_YIELD(&self.pt);

    self.idcode_version = self.idcode[0] >> 28;
    self.idcode_part_number = (self.idcode[0] >> 12) & 0xffff;
    self.idcode_manufid = (self.idcode[0] >> 1) & 0x7ff;
    self.idcode_lsb = self.idcode[0] & 0x1;

    // dtmcs

    self.new_reg[0] = 0x10;
    rvl_tap_shift_ir(self.old_reg, self.new_reg, 5);
    PT_YIELD(&self.pt);

    self.new_reg[0] = 0x00;
    rvl_tap_shift_dr(self.dtmcs, self.new_reg, 32);
    PT_YIELD(&self.pt);

    self.dtmcs_version = self.dtmcs[0] & 0xf;
    self.dtmcs_abits = (self.dtmcs[0] >> 4) & 0x3f;
    self.dtmcs_dmistat = (self.dtmcs[0] >> 10) & 0x3;
    self.dtmcs_idle = (self.dtmcs[0] >> 12) & 0x7;

    //
    for(;;) {
        rvl_tap_go_idle();
        PT_RESTART(&self.pt);
    }

    PT_END(&self.pt);
}

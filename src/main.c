#include "rvl_tap.h"

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

int main(void)
{
	rvl_tap_go_idle();

	// idcode
	new_reg[0] = 0x01;
	rvl_tap_shift_ir(old_reg, new_reg, 5);

	new_reg[0] = 0x00;
	rvl_tap_shift_dr(idcode, new_reg, 32);

	idcode_version = idcode[0] >> 28;
	idcode_part_number = (idcode[0] >> 12) & 0xffff;
	idcode_manufid = (idcode[0] >> 1) & 0x7ff;
	idcode_lsb = idcode[0] & 0x1;

	// dtmcs

	new_reg[0] = 0x10;
	rvl_tap_shift_ir(old_reg, new_reg, 5);

	new_reg[0] = 0x00;
	rvl_tap_shift_dr(dtmcs, new_reg, 32);

	dtmcs_version = dtmcs[0] & 0xf;
	dtmcs_abits = (dtmcs[0] >> 4) & 0x3f;
	dtmcs_dmistat = (dtmcs[0] >> 10) & 0x3;
	dtmcs_idle = (dtmcs[0] >> 12) & 0x7;

	//
	for(;;) {
		rvl_tap_go_idle();
	}

	return 0;
}

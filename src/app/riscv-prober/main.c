#if 0
#include "rvl_jtag.h"
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
	rvl_jtag_init();
	rvl_tap_go_idle();

	// idcode, FE310-G000's idcode: 0x10e31913 (mfg: 0x489 (SiFive, Inc.), part: 0x0e31, ver: 0x1)
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
#else

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint8_t packet_sent, packet_receive;
extern uint32_t receive_length;

usb_core_driver USB_OTG_dev =
{
    .dev = {
        .desc = {
            .dev_desc       = (uint8_t *)&device_descriptor,
            .config_desc    = (uint8_t *)&configuration_descriptor,
            .strings        = usbd_strings,
        }
    }
};


/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    eclic_global_interrupt_enable();

    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);

    usb_rcu_config();

    usb_timer_init();

    usb_intr_config();

    usbd_init (&USB_OTG_dev, USB_CORE_ENUM_FS, &usbd_cdc_cb);

    /* check if USB device is enumerated successfully */
    while (USBD_CONFIGURED != USB_OTG_dev.dev.cur_status) {
    }

    while (1) {
        if (USBD_CONFIGURED == USB_OTG_dev.dev.cur_status) {
            if (1 == packet_receive && 1 == packet_sent) {
                packet_sent = 0;
                /* receive datas from the host when the last packet datas have sent to the host */
                cdc_acm_data_receive(&USB_OTG_dev);
            } else {
                if (0 != receive_length) {
                    /* send receive datas */
                    cdc_acm_data_send(&USB_OTG_dev, receive_length);
                    receive_length = 0;
                }
            }
        }
    }
}
#endif


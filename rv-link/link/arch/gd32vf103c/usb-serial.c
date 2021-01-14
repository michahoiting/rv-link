/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/usb-serial.c
 * \brief Application defined handling of the CDC ACM USB driver and handling of
 *        CDC_ACM_DATA EP of the CDC ACM USB device.
 *
 * RV-LINK is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

/* own header file include */
#include <rv-link/link/usb-serial.h>

/* system library header file includes */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* other library header file includes */
#include <gd32vf103-sdk/GD32VF103_usbfs_driver/Include/usbd_enum.h>
#include <pt/pt.h>

/* other project header file includes */
#include <rv-link/gdb-server/gdb-packet.h>

/* own component header file includes */
#include <rv-link/link/arch/gd32vf103c/details/cdc_acm_core.h>
#include <rv-link/link/arch/gd32vf103c/details/gd32vf103_hw.h>
#include <rv-link/link/led.h>

usb_core_driver USB_OTG_dev = {0};

typedef struct rvl_usb_serial_s
{
    struct pt pt_poll;
    struct pt pt_recv;
    struct pt pt_send;
    const uint8_t *send_buffer;
    size_t send_len;
} rvl_usb_serial_t;

static rvl_usb_serial_t rvl_usb_serial_i;
#define self rvl_usb_serial_i

static uint8_t usb_serial_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];

static PT_THREAD(rvl_usb_serial_recv_poll(void));
static PT_THREAD(rvl_usb_serial_send_poll(void));


void rvl_usb_serial_init(void)
{
    PT_INIT(&self.pt_poll);
    PT_INIT(&self.pt_recv);
    PT_INIT(&self.pt_send);

    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);

    usb_rcu_config();
    usb_timer_init();
    usb_intr_config();

    cdc_acm_init_desc(&USB_OTG_dev.dev.desc);

    usbd_init(&USB_OTG_dev, USB_CORE_ENUM_FS, (usb_class_core*) &cdc_acm_usb_class_core);
}


PT_THREAD(rvl_usb_serial_poll(void))
{
    PT_BEGIN(&self.pt_poll);

    (void) PT_SCHEDULE(rvl_usb_serial_recv_poll());
    (void) PT_SCHEDULE(rvl_usb_serial_send_poll());

    PT_END(&self.pt_poll);
}


static PT_THREAD(rvl_usb_serial_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv);

    PT_WAIT_UNTIL(&self.pt_recv, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        cdc_acm_ep0_packet_received = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM_DATA_OUT_EP, (uint8_t*)(usb_serial_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv, cdc_acm_ep0_packet_received);
        rvl_led_indicator(RVL_LED_INDICATOR_LINK_USB, true);

        PT_WAIT_UNTIL(&self.pt_recv, gdb_packet_process_command(usb_serial_recv_buffer, cdc_acm_ep0_packet_length));
    }

    PT_END(&self.pt_recv);
}


PT_THREAD(rvl_usb_serial_send_poll(void))
{
    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        PT_WAIT_UNTIL(&self.pt_send, (self.send_buffer = gdb_packet_get_response(&self.send_len)) != NULL);

        cdc_acm_ep0_packet_sent = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM_DATA_IN_EP, (uint8_t*)(self.send_buffer), self.send_len);

        PT_WAIT_UNTIL(&self.pt_send, cdc_acm_ep0_packet_sent);
        rvl_led_indicator(RVL_LED_INDICATOR_LINK_USB, true);

        gdb_packet_release_response();
    }

    PT_END(&self.pt_send);
}

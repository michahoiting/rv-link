/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2021, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/usb-serial2.c
 * \brief Handling of CDC_ACM2_DATA EP of the CDC ACM USB device.
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
#include <stdint.h>

/* other library header file includes */
#include <gd32vf103-sdk/GD32VF103_usbfs_driver/Include/usbd_enum.h>
#include <pt/pt.h>

/* own component header file includes */
#include <rv-link/link/arch/gd32vf103c/details/cdc_acm_core.h>

extern usb_core_driver USB_OTG_dev;

typedef struct rvl_usb_serial2_s
{
    struct pt pt_poll;
    struct pt pt_recv;
    struct pt pt_send;
    uint8_t usb_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
    volatile bool usb_recv_ready;
} rvl_usb_serial2_t;

static rvl_usb_serial2_t rvl_usb_serial2_i;
#define self rvl_usb_serial2_i

static PT_THREAD(rvl_usb_serial2_recv_poll(void));
static PT_THREAD(rvl_usb_serial2_send_poll(void));


void rvl_usb_serial2_init(void)
{
    PT_INIT(&self.pt_poll);
    PT_INIT(&self.pt_recv);
    PT_INIT(&self.pt_send);
    self.usb_recv_ready = false;
}


PT_THREAD(rvl_usb_serial2_poll(void))
{
    PT_BEGIN(&self.pt_poll);

    (void) PT_SCHEDULE(rvl_usb_serial2_recv_poll());
    (void) PT_SCHEDULE(rvl_usb_serial2_send_poll());

    PT_END(&self.pt_poll);
}


static PT_THREAD(rvl_usb_serial2_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv);

    PT_WAIT_UNTIL(&self.pt_recv, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        cdc_acm_ep2_packet_received = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM2_DATA_OUT_EP, (uint8_t*)(self.usb_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv, cdc_acm_ep2_packet_received);
        self.usb_recv_ready = true;
    }

    PT_END(&self.pt_recv);
}


static PT_THREAD(rvl_usb_serial2_send_poll(void))
{
    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        PT_WAIT_UNTIL(&self.pt_send, self.usb_recv_ready);
        self.usb_recv_ready = false;

        cdc_acm_ep2_packet_sent = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM2_DATA_IN_EP, (uint8_t*)(self.usb_recv_buffer), cdc_acm_ep2_packet_length);
        PT_WAIT_UNTIL(&self.pt_send, cdc_acm_ep2_packet_sent);
    }

    PT_END(&self.pt_send);
}

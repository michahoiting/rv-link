/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/usb-serial1.c
 * \brief Handling of CDC_ACM1_DATA EP of the CDC ACM USB device.
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
#include <rv-link/link/serial.h>

extern usb_core_driver USB_OTG_dev;

typedef struct usb_serial1_s
{
    struct pt pt_poll;
    struct pt pt_recv;
    struct pt pt_send;
    int i_usb_recv_buffer;
    uint8_t usb_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
    uint8_t usb_send_buffer[CDC_ACM_DATA_PACKET_SIZE];
} usb_serial1_t;

static usb_serial1_t usb_serial1_i;
#define self usb_serial1_i

PT_THREAD(rvl_usb_serial1_recv_poll(void));
PT_THREAD(rvl_usb_serial1_send_poll(void));


void rvl_usb_serial1_init(void)
{
    PT_INIT(&self.pt_poll);
    PT_INIT(&self.pt_recv);
    PT_INIT(&self.pt_send);

    /* initialize the UART to the target machine */
    rvl_serial_init();
}


PT_THREAD(rvl_usb_serial1_poll(void))
{
    PT_BEGIN(&self.pt_poll);

    (void) PT_SCHEDULE(rvl_usb_serial1_recv_poll());
    (void) PT_SCHEDULE(rvl_usb_serial1_send_poll());

    PT_END(&self.pt_poll);
}


PT_THREAD(rvl_usb_serial1_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv);

    PT_WAIT_UNTIL(&self.pt_recv, USB_OTG_dev.dev.cur_status == USBD_CONFIGURED);

    for (;;) {
        cdc_acm_ep1_packet_received = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM1_DATA_OUT_EP, (uint8_t*) self.usb_recv_buffer, CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv, cdc_acm_ep1_packet_received);

        for (self.i_usb_recv_buffer = 0; self.i_usb_recv_buffer < cdc_acm_ep1_packet_length; self.i_usb_recv_buffer++) {
            PT_WAIT_THREAD(&self.pt_recv, rvl_serial_putchar(self.usb_recv_buffer[self.i_usb_recv_buffer]));
        }
    }

    PT_END(&self.pt_recv);
}


PT_THREAD(rvl_usb_serial1_send_poll(void))
{
    uint8_t c;
    int i;

    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        PT_WAIT_THREAD(&self.pt_send, rvl_serial_getchar(&c));
        i = 0;
        do {
            self.usb_send_buffer[i] = c;
            i++;
            if (i >= sizeof(self.usb_send_buffer)) {
                break;
            }
            /* Continue collecting received characters until pt blocks */
        } while (!PT_SCHEDULE(rvl_serial_getchar(&c)));

        cdc_acm_ep1_packet_sent = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM1_DATA_IN_EP, (uint8_t*)(self.usb_send_buffer), i);

        PT_WAIT_UNTIL(&self.pt_send, cdc_acm_ep1_packet_sent);
    }

    PT_END(&self.pt_send);
}

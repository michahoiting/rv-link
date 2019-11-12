/*
Copyright (c) 2019 zoomdy@163.com
RV-LINK is licensed under the Mulan PSL v1.
You can use this software according to the terms and conditions of the Mulan PSL v1.
You may obtain a copy of Mulan PSL v1 at:
    http://license.coscl.org.cn/MulanPSL
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
PURPOSE.
See the Mulan PSL v1 for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include "pt/pt.h"
#include "rvl-usb-serial.h"

extern uint8_t packet_sent2, packet_receive2;
extern uint32_t receive_length2;
extern usb_core_driver USB_OTG_dev;


typedef struct usb_serial2_s
{
    struct pt pt_recv2;
    struct pt pt_send2;
}usb_serial2_t;

static usb_serial2_t usb_serial2_i;
#define self usb_serial2_i

static uint8_t usb_serial2_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
volatile bool serial2_data_ready = false;

PT_THREAD(usb_serial2_recv_poll(void));
PT_THREAD(usb_serial2_send_poll(void));


void usb_serial2_init(void)
{
    PT_INIT(&self.pt_recv2);
    PT_INIT(&self.pt_send2);
}


void usb_serial2_poll(void)
{
    usb_serial2_recv_poll();
    usb_serial2_send_poll();
}


PT_THREAD(usb_serial2_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv2);

    PT_WAIT_UNTIL(&self.pt_recv2, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        packet_receive2 = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM2_DATA_OUT_EP, (uint8_t*)(usb_serial2_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv2, packet_receive2 == 1);
        serial2_data_ready = true;
    }

    PT_END(&self.pt_recv2);
}


PT_THREAD(usb_serial2_send_poll(void))
{
    PT_BEGIN(&self.pt_send2);

    PT_WAIT_UNTIL(&self.pt_send2, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        PT_WAIT_UNTIL(&self.pt_send2, serial2_data_ready == true);
        serial2_data_ready = false;

        packet_sent2 = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM2_DATA_IN_EP, (uint8_t*)(usb_serial2_recv_buffer), receive_length2);

        PT_WAIT_UNTIL(&self.pt_send2, packet_sent2 == 1);
    }

    PT_END(&self.pt_send2);
}


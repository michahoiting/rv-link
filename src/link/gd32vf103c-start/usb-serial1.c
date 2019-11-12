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

extern uint8_t packet_sent1, packet_receive1;
extern uint32_t receive_length1;
extern usb_core_driver USB_OTG_dev;


typedef struct usb_serial1_s
{
    struct pt pt_recv1;
    struct pt pt_send1;
}usb_serial1_t;

static usb_serial1_t usb_serial1_i;
#define self usb_serial1_i

static uint8_t usb_serial1_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
volatile bool serial1_data_ready = false;

PT_THREAD(usb_serial1_recv_poll(void));
PT_THREAD(usb_serial1_send_poll(void));


void usb_serial1_init(void)
{
    PT_INIT(&self.pt_recv1);
    PT_INIT(&self.pt_send1);
}


void usb_serial1_poll(void)
{
    usb_serial1_recv_poll();
    usb_serial1_send_poll();
}


PT_THREAD(usb_serial1_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv1);

    PT_WAIT_UNTIL(&self.pt_recv1, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        packet_receive1 = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM1_DATA_OUT_EP, (uint8_t*)(usb_serial1_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv1, packet_receive1 == 1);
        serial1_data_ready = true;
    }

    PT_END(&self.pt_recv1);
}


PT_THREAD(usb_serial1_send_poll(void))
{
    PT_BEGIN(&self.pt_send1);

    PT_WAIT_UNTIL(&self.pt_send1, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        PT_WAIT_UNTIL(&self.pt_send1, serial1_data_ready == true);
        serial1_data_ready = false;

        packet_sent1 = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM1_DATA_IN_EP, (uint8_t*)(usb_serial1_recv_buffer), receive_length1);

        PT_WAIT_UNTIL(&self.pt_send1, packet_sent1 == 1);
    }

    PT_END(&self.pt_send1);
}


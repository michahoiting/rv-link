

/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  TODO
 * \brief TODO
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* other library header file includes */
#include <gd32vf103-sdk/GD32VF103_usbfs_driver/Include/drv_usb_hw.h>
#include <pt/pt.h>

/* other project header file includes */
#include <rv-link/gdb-server/gdb-packet.h>

/* own component header file includes */
#include <rv-link/link/arch/gd32vf103c/details/cdc_acm_core.h>


int rvl_vcom_enable(void);

usb_core_driver USB_OTG_dev;

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

PT_THREAD(rvl_usb_serial_recv_poll(void));
PT_THREAD(rvl_usb_serial_send_poll(void));

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

    int vcom = rvl_vcom_enable(); /* TODO: make sure gdb-server has been initialized */
    USB_OTG_dev.dev.desc.dev_desc = (uint8_t*)&device_descriptor;
    USB_OTG_dev.dev.desc.config_desc = 
        vcom ? (uint8_t *) &configuration_descriptor_vcom_enable:
               (uint8_t *) &configuration_descriptor_vcom_disable;
    USB_OTG_dev.dev.desc.strings =
        vcom ? usbd_strings_vcom_enable:usbd_strings_vcom_disable;
    usbd_init(&USB_OTG_dev, USB_CORE_ENUM_FS, &usbd_cdc_cb);
}


PT_THREAD(rvl_usb_serial_poll(void))
{
    PT_BEGIN(&self.pt_poll);

    (void) PT_SCHEDULE(rvl_usb_serial_recv_poll());
    (void) PT_SCHEDULE(rvl_usb_serial_send_poll());

    PT_END(&self.pt_poll);
}


PT_THREAD(rvl_usb_serial_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv);

    PT_WAIT_UNTIL(&self.pt_recv, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        cdc_acm0_packet_received = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM_DATA_OUT_EP, (uint8_t*)(usb_serial_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv, cdc_acm0_packet_received);

        PT_WAIT_UNTIL(&self.pt_recv, gdb_packet_process_command(usb_serial_recv_buffer, cdc_acm0_packet_length));
    }

    PT_END(&self.pt_recv);
}


PT_THREAD(rvl_usb_serial_send_poll(void))
{
    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        PT_WAIT_UNTIL(&self.pt_send, (self.send_buffer = gdb_packet_get_response(&self.send_len)) != NULL);

        cdc_acm0_packet_sent = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM_DATA_IN_EP, (uint8_t*)(self.send_buffer), self.send_len);

        PT_WAIT_UNTIL(&self.pt_send, cdc_acm0_packet_sent);
        gdb_packet_release_response();
    }

    PT_END(&self.pt_send);
}

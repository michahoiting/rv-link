#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include "pt/pt.h"
#include "rvl-usb-serial.h"

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

typedef struct usb_serial_s
{
    struct pt pt_recv;
    struct pt pt_send;
    const uint8_t *send_buffer;
    size_t send_len;
}usb_serial_t;

static usb_serial_t usb_serial_i;
#define self usb_serial_i

static uint8_t usb_serial_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];


void usb_serial_init(void)
{
    PT_INIT(&self.pt_recv);
    PT_INIT(&self.pt_send);

    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);

    usb_rcu_config();
    usb_timer_init();
    usb_intr_config();
    usbd_init (&USB_OTG_dev, USB_CORE_ENUM_FS, &usbd_cdc_cb);
}


PT_THREAD(usb_serial_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv);

    PT_WAIT_UNTIL(&self.pt_recv, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        packet_receive = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM_DATA_OUT_EP, (uint8_t*)(usb_serial_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv, packet_receive == 1);

        PT_WAIT_UNTIL(&self.pt_recv, usb_serial_put_recv_data(usb_serial_recv_buffer, receive_length));
    }

    PT_END(&self.pt_recv);
}


PT_THREAD(usb_serial_send_poll(void))
{
    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        PT_WAIT_UNTIL(&self.pt_send, (self.send_buffer = usb_serial_get_send_data(&self.send_len)) != NULL);

        packet_sent = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM_DATA_IN_EP, (uint8_t*)(self.send_buffer), self.send_len);

        PT_WAIT_UNTIL(&self.pt_send, packet_sent == 1);
        usb_serial_data_sent();
    }

    PT_END(&self.pt_send);
}


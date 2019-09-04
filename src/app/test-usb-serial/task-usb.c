#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include "pt.h"


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

typedef struct task_usb_s
{
    struct pt pt;
}task_usb_t;

static task_usb_t task_usb_i;
#define self task_usb_i

void task_usb_init(void)
{
    PT_INIT(&self.pt);

    eclic_global_interrupt_enable();

    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);

    usb_rcu_config();

    usb_timer_init();

    usb_intr_config();

    usbd_init (&USB_OTG_dev, USB_CORE_ENUM_FS, &usbd_cdc_cb);
}

uint8_t usb_serial_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
uint8_t usb_send_buffer[CDC_ACM_DATA_PACKET_SIZE * 2];

PT_THREAD(task_usb_poll(void))
{
    uint32_t i;
    uint32_t len_recv;
    uint32_t len_send;
    uint8_t *p;
    uint8_t *q;

    PT_BEGIN(&self.pt);

    PT_WAIT_UNTIL(&self.pt, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

//    /* check if USB device is enumerated successfully */
//    while (USBD_CONFIGURED != USB_OTG_dev.dev.cur_status) {
//    }


    while (1) {
        packet_receive = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM_DATA_OUT_EP, (uint8_t*)(usb_serial_recv_buffer), CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt, packet_receive == 1);

        len_recv = receive_length;
        p = usb_serial_recv_buffer;
        q = usb_send_buffer;
        len_send = 0;
        for(i = 0; i < len_recv; i++) {
            if(p[i] >= 'A' && p[i] <= 'Z') {
                q[len_send] = p[i] + ('a' - 'A');
            } else if(p[i] >= 'a' && p[i] <= 'z') {
                q[len_send] = p[i] - ('a' - 'A');
            } else if(p[i] == '\r'){
                q[len_send] = '\r';
                len_send++;
                q[len_send] = '\n';
            } else {
                q[len_send] = p[i];
            }
            len_send++;
        }

        packet_sent = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM_DATA_IN_EP, (uint8_t*)(usb_send_buffer), len_send);

        PT_WAIT_UNTIL(&self.pt, packet_sent == 1);
    }
#if 0
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
        PT_YIELD(&self.pt);
    }
#endif
    PT_END(&self.pt);
}

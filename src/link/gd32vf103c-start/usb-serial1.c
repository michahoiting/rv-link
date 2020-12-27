/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020 Micha Hoiting <micha.hoiting@gmail.com>
 * RV-LINK is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include "rvl-usb-serial.h"

#include "cdc_acm_core.h"
#include "drv_usb_hw.h"
#include "pt/pt.h"

#ifdef LINK_LONGAN_NANO
#include "gd32vf103_longan_nano.h"
#endif

extern uint8_t packet_sent_cdc_acm1;
extern uint8_t packet_received_cdc_acm1;
extern uint32_t packet_length_cdc_acm1;
extern usb_core_driver USB_OTG_dev;

typedef struct buffer_s
{
    uint8_t buffer[CDC_ACM_DATA_PACKET_SIZE];
    int head;
    int tail;
} buffer_t;

typedef struct usb_serial1_s
{
    struct pt pt_recv;
    struct pt pt_send;
    int i_usb_recv_buffer;
    uint8_t usb_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
    uint8_t usb_send_buffer[CDC_ACM_DATA_PACKET_SIZE];
    buffer_t uart_recv_buf;
} usb_serial1_t;

static usb_serial1_t usb_serial1_i;
#define self usb_serial1_i


PT_THREAD(usb_serial1_recv_poll(void));
PT_THREAD(usb_serial1_send_poll(void));

static void serial1_init(void);
static bool usb_serial1_uart_xmit(uint8_t c);
static void usb_serial1_uart_recv_buf_put(uint8_t c);
static bool usb_serial1_uart_recv_buf_get(uint8_t* c);


void usb_serial1_init(void)
{
    PT_INIT(&self.pt_recv);
    PT_INIT(&self.pt_send);

    serial1_init();
}


void usb_serial1_poll(void)
{
    usb_serial1_recv_poll();
    usb_serial1_send_poll();
}


PT_THREAD(usb_serial1_recv_poll(void))
{
    PT_BEGIN(&self.pt_recv);

    PT_WAIT_UNTIL(&self.pt_recv, USB_OTG_dev.dev.cur_status == USBD_CONFIGURED);

    for (;;) {
        packet_received_cdc_acm1 = 0;
        usbd_ep_recev(&USB_OTG_dev, CDC_ACM1_DATA_OUT_EP, (uint8_t*) self.usb_recv_buffer, CDC_ACM_DATA_PACKET_SIZE);

        PT_WAIT_UNTIL(&self.pt_recv, packet_received_cdc_acm1);

        for (self.i_usb_recv_buffer = 0; self.i_usb_recv_buffer < packet_length_cdc_acm1; self.i_usb_recv_buffer++) {
            PT_WAIT_UNTIL(&self.pt_recv, usb_serial1_uart_xmit(self.usb_recv_buffer[self.i_usb_recv_buffer]));
        }
    }

    PT_END(&self.pt_recv);
}


PT_THREAD(usb_serial1_send_poll(void))
{
    uint8_t c;
    int i;

    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    for (;;) {
        PT_WAIT_UNTIL(&self.pt_send, usb_serial1_uart_recv_buf_get(&c));
        i = 0;
        do {
            self.usb_send_buffer[i] = c;
            i++;
            if (i >= sizeof(self.usb_send_buffer)) {
                break;
            }
        } while(usb_serial1_uart_recv_buf_get(&c));

        packet_sent_cdc_acm1 = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM1_DATA_IN_EP, (uint8_t*)(self.usb_send_buffer), i);

        PT_WAIT_UNTIL(&self.pt_send, packet_sent_cdc_acm1);
    }

    PT_END(&self.pt_send);
}


static void serial1_init(void)
{
    self.uart_recv_buf.tail = 0;
    self.uart_recv_buf.head = 0;

    gd_longan_nano_uart_init(UART_0, UART_INTMODE_ENABLE);
}


void serial1_set_line_coding(uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity)
{
    usart_disable(USART0);
    usart_baudrate_set(USART0, baudrate);
    usart_enable(USART0);
}


static bool usb_serial1_uart_xmit(uint8_t c)
{
    if (usart_flag_get(USART0, USART_FLAG_TBE)) {
        usart_data_transmit(USART0, c);
        return true;
    } else {
        return false;
    }
}


static bool usb_serial1_uart_recv_buf_get(uint8_t* c)
{
    int tail;
    if (self.uart_recv_buf.tail == self.uart_recv_buf.head) {
        return false;
    } else {
        tail = self.uart_recv_buf.tail + 1;
        tail = tail >= sizeof(self.uart_recv_buf.buffer) ? 0 : tail;
        *c = self.uart_recv_buf.buffer[self.uart_recv_buf.tail];
        asm volatile ("":::"memory");
        self.uart_recv_buf.tail = tail;
        return true;
    }
}


static void usb_serial1_uart_recv_buf_put(uint8_t c)
{
    int head = self.uart_recv_buf.head + 1;
    head = head >= sizeof(self.uart_recv_buf.buffer) ? 0 : head;
    if (self.uart_recv_buf.tail != head) {
        self.uart_recv_buf.buffer[self.uart_recv_buf.head] = c;
        asm volatile ("":::"memory");
        self.uart_recv_buf.head = head;
    }
}


void USART0_IRQHandler(void)
{
    uint8_t c;
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET) {
        /* read one byte from the receive data register */
        c = (uint8_t)usart_data_receive(USART0);
        usb_serial1_uart_recv_buf_put(c);
    }
}

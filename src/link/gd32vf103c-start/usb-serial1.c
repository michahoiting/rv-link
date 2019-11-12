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
    int i_recv;
}usb_serial1_t;

static usb_serial1_t usb_serial1_i;
#define self usb_serial1_i

static uint8_t usb_serial1_recv_buffer[CDC_ACM_DATA_PACKET_SIZE];
static uint8_t usb_serial1_send_buffer[CDC_ACM_DATA_PACKET_SIZE];
volatile bool serial1_data_ready = false;

PT_THREAD(usb_serial1_recv_poll(void));
PT_THREAD(usb_serial1_send_poll(void));

static void serial1_init(void);
static bool serial1_putchar(uint8_t c);
static bool serial1_getchar(uint8_t* c);


void usb_serial1_init(void)
{
    PT_INIT(&self.pt_recv1);
    PT_INIT(&self.pt_send1);

    serial1_init();
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

        for(self.i_recv = 0; self.i_recv < receive_length1; self.i_recv++) {
            PT_WAIT_UNTIL(&self.pt_recv1, serial1_putchar(usb_serial1_recv_buffer[self.i_recv]));
        }
        serial1_data_ready = true;
    }

    PT_END(&self.pt_recv1);
}


PT_THREAD(usb_serial1_send_poll(void))
{
    uint8_t c;
    int i;

    PT_BEGIN(&self.pt_send1);

    PT_WAIT_UNTIL(&self.pt_send1, USBD_CONFIGURED == USB_OTG_dev.dev.cur_status);

    while (1) {
        PT_WAIT_UNTIL(&self.pt_send1, serial1_getchar(&c));
        i = 0;
        do {
            usb_serial1_send_buffer[i] = c;
            i++;
            if(i >= sizeof(usb_serial1_send_buffer)) {
                break;
            }
        } while(serial1_getchar(&c));

        packet_sent1 = 0;
        usbd_ep_send(&USB_OTG_dev, CDC_ACM1_DATA_IN_EP, (uint8_t*)(usb_serial1_send_buffer), i);

        PT_WAIT_UNTIL(&self.pt_send1, packet_sent1 == 1);
    }

    PT_END(&self.pt_send1);
}


typedef struct serial_buffer_s
{
    uint8_t buffer[CDC_ACM_DATA_PACKET_SIZE];
    int head;
    int tail;
}serial_buffer_t;


static serial_buffer_t serial1_buffer;

static void serial1_init(void)
{
    serial1_buffer.tail = 0;
    serial1_buffer.head = 0;

#if defined(LINK_LONGAN_NANO)
    /*
     * USART0_REMAP = 0
     * TX0: PA9
     * RX0: PA10
     */
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
#elif defined(LINK_LONGAN_NANO_ALT) || defined(LINK_GD32VF103C_START)
    /*
     * USART0_REMAP = 1
     * TX0: PB6
     * RX0: PB7
     */
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_6);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, GPIO_PIN_7);
#else
#error "FIXME"
#endif

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

    /* enable USART0 receive interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    eclic_irq_enable(USART0_IRQn, 1, 0);
}


void serial1_set_line_coding(uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity)
{
    usart_disable(USART0);

    usart_baudrate_set(USART0, baudrate);

    usart_enable(USART0);
}


static bool serial1_putchar(uint8_t c)
{
    if(usart_flag_get(USART0, USART_FLAG_TBE)) {
        usart_data_transmit(USART0, c);
        return true;
    } else {
        return false;
    }
}


static void serial1_buffer_put(uint8_t c)
{
    int head = serial1_buffer.head + 1;
    head = head >= sizeof(serial1_buffer.buffer) ? 0 : head;
    if(serial1_buffer.tail != head) {
        serial1_buffer.buffer[serial1_buffer.head] = c;
        asm volatile ("":::"memory");
        serial1_buffer.head = head;
    }
}


static bool serial1_getchar(uint8_t* c)
{
    int tail;
    if(serial1_buffer.tail == serial1_buffer.head) {
        return false;
    } else {
        tail = serial1_buffer.tail + 1;
        tail = tail >= sizeof(serial1_buffer.buffer) ? 0 : tail;
        *c = serial1_buffer.buffer[serial1_buffer.tail];
        asm volatile ("":::"memory");
        serial1_buffer.tail = tail;
        return true;
    }
}


void USART0_IRQHandler(void)
{
    uint8_t c;

    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* read one byte from the receive data register */
        c = (uint8_t)usart_data_receive(USART0);
        serial1_buffer_put(c);
    }
}

/**
 *     Copyright (c) 2020, Micha Hoiting.
 *
 *     \file    rvl-uart-serial2.c
 *     \author  Micha Hoiting <micha.hoiting@gmail.com>
 *     \brief   Functions to handle the GD32VF103 UART2
 */

#include "rvl-uart-serial.h"

#include "gd32vf103.h"
#include "gd32vf103_longan_nano.h"

#define UART_SERIAL2_BUFFER_SIZE 64

typedef struct buffer_s
{
    uint8_t buffer[UART_SERIAL2_BUFFER_SIZE];
    int head;
    int tail;
} buffer_t;

typedef struct uart_serial2_s
{
    struct pt pt_send;
    buffer_t recv_buffer;
    const uint8_t* xmit_buffer;
} uart_serial2_t;

static uart_serial2_t uart_serial2_i;
#define self uart_serial2_i

static bool uart_serial2_data_received(uint8_t c);
static bool uart_serial2_data_xmit(uint8_t c);


void uart_serial2_init(void)
{
    PT_INIT(&self.pt_send);

    self.recv_buffer.tail = 0;
    self.recv_buffer.head = 0;
    self.xmit_buffer = NULL;

    gd_longan_nano_uart_init(UART_2, UART_INTMODE_ENABLE);
}


PT_THREAD(uart_serial2_puts(const char* s))
{
    PT_BEGIN(&self.pt_send);

    self.xmit_buffer = (uint8_t*)s;
 
    while (*self.xmit_buffer) {
        PT_WAIT_UNTIL(&self.pt_send, uart_serial2_data_xmit(*self.xmit_buffer));
        self.xmit_buffer++;
    }

    PT_END(&self.pt_send);
}


bool uart_serial2_getchar(char* c)
{
    int tail;
    if (self.recv_buffer.tail == self.recv_buffer.head) {
        return false;
    } else {
        tail = self.recv_buffer.tail + 1;
        tail = tail >= sizeof(self.recv_buffer.buffer) ? 0 : tail;
        *c = self.recv_buffer.buffer[self.recv_buffer.tail];
        asm volatile ("":::"memory");
        self.recv_buffer.tail = tail;
        return true;
    }
}


static bool uart_serial2_data_xmit(uint8_t c)
{
    if (usart_flag_get(USART2, USART_FLAG_TBE)) {
        usart_data_transmit(USART2, c);
        return true;
    } else {
        return false;
    }
}


static bool uart_serial2_data_received(uint8_t c)
{
    int head = self.recv_buffer.head + 1;
    head = head >= sizeof(self.recv_buffer.buffer) ? 0 : head;
    if (self.recv_buffer.tail != head) {
        self.recv_buffer.buffer[self.recv_buffer.head] = c;
        asm volatile ("":::"memory");
        self.recv_buffer.head = head;
        return true;
    }
    return false;
}


void USART2_IRQHandler(void)
{
    uint8_t c;
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) != RESET) {
        /* read one byte from the receive data register */
        c = (uint8_t)usart_data_receive(USART2);
        (void) uart_serial2_data_received(c);
    }
}

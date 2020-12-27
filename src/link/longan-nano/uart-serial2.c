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

typedef struct serial_buffer_s
{
    uint8_t buffer[UART_SERIAL2_BUFFER_SIZE];
    int head;
    int tail;
} serial_buffer_t;

typedef struct uart_serial2_s
{
    struct pt pt_send;
    serial_buffer_t in_buffer;
    const uint8_t* out_buffer;
} uart_serial2_t;

static uart_serial2_t uart_serial2_i;
#define self uart_serial2_i

static bool uart_serial2_data_recv(uint8_t c);
static bool uart_serial2_data_xmit(uint8_t c);


void uart_serial2_init(void)
{
    PT_INIT(&self.pt_send);

    self.in_buffer.tail = 0;
    self.in_buffer.head = 0;
    self.out_buffer = NULL;

    gd_longan_nano_uart_init(UART_2, UART_INTMODE_ENABLE);
}


PT_THREAD(uart_serial2_puts(const char* s))
{
    PT_BEGIN(&self.pt_send);

    self.out_buffer = (uint8_t*)s;
 
    while (*self.out_buffer) {
        PT_WAIT_UNTIL(&self.pt_send, uart_serial2_data_xmit(*self.out_buffer));
        self.out_buffer++;
    }

    PT_END(&self.pt_send);
}


bool uart_serial2_getchar(char* c)
{
    int tail;
    if (self.in_buffer.tail == self.in_buffer.head) {
        return false;
    } else {
        tail = self.in_buffer.tail + 1;
        tail = tail >= sizeof(self.in_buffer.buffer) ? 0 : tail;
        *c = self.in_buffer.buffer[self.in_buffer.tail];
        asm volatile ("":::"memory");
        self.in_buffer.tail = tail;
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


static bool uart_serial2_data_recv(uint8_t c)
{
    int head = self.in_buffer.head + 1;
    head = head >= sizeof(self.in_buffer.buffer) ? 0 : head;
    if (self.in_buffer.tail != head) {
        self.in_buffer.buffer[self.in_buffer.head] = c;
        asm volatile ("":::"memory");
        self.in_buffer.head = head;
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
        (void) uart_serial2_data_recv(c);
    }
}

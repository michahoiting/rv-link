/**
 *     Copyright (c) 2020, Micha Hoiting.
 *
 *     \file    rvl-uart-serial.h
 *     \author  Micha Hoiting <micha.hoiting@gmail.com>
 *     \brief   Functions to handle the GD32VF103 UART2
 */

#ifndef __RVL_UART_SERIAL_H__
#define __RVL_UART_SERIAL_H__

#include <stdbool.h>
#include <stdint.h>

#include "pt/pt.h"

void uart_serial2_init(void);
PT_THREAD(uart_serial2_puts(const char* s));
bool uart_serial2_getchar(char* c);

#endif /* __RVL_UART_SERIAL_H__ */

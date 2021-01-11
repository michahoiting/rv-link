#ifndef __RV_LINK_LINK_SERIAL_H__
#define __RV_LINK_LINK_SERIAL_H__
/**
 *     Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 *     \file    rv-link/link/serial.h
 *     \brief   Functions to handle the  USART connected to the target machine.
 *              The rvl_usb_serial1 realizes the serial connection with a
 *              (configurable) USART of target machine, and uses it as a USB
 *              virtual serial port.
 */

/* system library header file includes */
#include <stdint.h>

/* other library header file includes */
#include <pt/pt.h>


void rvl_serial_init(void);
void rvl_serial_set_line_coding(uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity);

PT_THREAD(rvl_serial_getchar(uint8_t* c));
PT_THREAD(rvl_serial_putchar(uint8_t c));

#endif /* __RV_LINK_LINK_SERIAL_H__ */

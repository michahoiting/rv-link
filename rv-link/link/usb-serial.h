#ifndef __RV_LINK_LINK_USB_SERIAL_H__
#define __RV_LINK_LINK_USB_SERIAL_H__
/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/usb-serial.h
 * \brief Handling of USB CDC ACM virtual serial ports.
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

/* system library header file includes */
#include <pt/pt.h>

/*
 * The following functions are called by the main function of app and
 * implemented by the USB Serial component of link:
 *
 * - rvl_usb_serial_*
 *      corresponds to the first serial port /dev/ttyACM0 to
 *      realize GDB server and connect to GDB;
 */
void rvl_usb_serial_init(void);
PT_THREAD(rvl_usb_serial_poll(void));

/* - rvl_usb_serial1_*
 *      corresponds to the second serial port /dev/ttyACM1,
 *      realizes the serial connection with a (configurable)
 *      UART of target machine, and uses it as a virtual serial port;
 */
void rvl_usb_serial1_init(void);
PT_THREAD(rvl_usb_serial1_poll(void));

/* - rvl_usb_serial2_*
 *      corresponds to the third serial port /dev/ttyACM2, the standard input
 *      and output of RV-LINK are redirected to this serial port.
 */
void rvl_usb_serial2_init(void);
PT_THREAD(rvl_usb_serial2_poll(void));

#endif /* __RV_LINK_LINK_USB_SERIAL_H__ */

/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/main.c
 * \brief Main RV-Link application.
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

/* other library header file includes */
#include <pt/pt.h>

/* other project header file includes */
#include <rv-link/gdb-server/gdb-packet.h>
#include <rv-link/gdb-server/gdb-server.h>
#include <rv-link/link/can.h>
#include <rv-link/link/led.h>
#include <rv-link/link/usb-serial.h>

int main(void)
{
    rvl_led_init();
    rvl_can_init();

    /* Due to nvm-config init-dependency, the gdb server must be initialized first */
    gdb_server_init();

    rvl_usb_serial_init();
    rvl_usb_serial1_init();

    for(;;) {
        (void) PT_SCHEDULE(rvl_led_poll());
        (void) PT_SCHEDULE(rvl_usb_serial_poll());
        (void) PT_SCHEDULE(rvl_usb_serial1_poll());
        (void) PT_SCHEDULE(gdb_server_poll());
    }
}

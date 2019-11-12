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
#include "rvl-led.h"
#include "rvl-serial.h"
#include "rvl-usb-serial.h"
#include "gdb-serial.h"
#include "gdb-server.h"


int main(void)
{
    rvl_led_init();
    rvl_serial_init();
    usb_serial_init();
    usb_serial1_init();
    usb_serial2_init();
    gdb_serial_init();
    gdb_server_init();


    for(;;) {
        rvl_led_poll();
        rvl_serial_poll();
        usb_serial_poll();
        gdb_server_poll();
        usb_serial1_poll();
        usb_serial2_poll();
    }
}

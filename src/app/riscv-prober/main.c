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
#include "pt/pt.h"
#include "rvl-led.h"
#include "rvl-usb-serial.h"

void task_probe_init(void);
PT_THREAD(task_probe_poll(void));

int main(void)
{
    rvl_led_init();
    usb_serial_init();
    task_probe_init();

    for(;;) {
        usb_serial_recv_poll();
        usb_serial_send_poll();
        task_probe_poll();
    }

    return 0;
}



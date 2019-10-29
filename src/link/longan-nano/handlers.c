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
#include <stdint.h>

#include "rvl-led.h"
#include "pt/timer.h"


uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp)
{
    struct timer timer;

    timer_set(&timer, 100 * 1000 /* 100 ms */);

    for(;;) {
        rvl_led_assert(1);
        while(!timer_expired(&timer));
        timer_reset(&timer);

        rvl_led_assert(0);
        while(!timer_expired(&timer));
        timer_reset(&timer);
    }

    return 0;
}

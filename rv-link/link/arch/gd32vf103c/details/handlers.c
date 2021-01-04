/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/longan-nano/details/handlers.c
 * \brief Implementation of the GD32VF103C trap handler.
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

/* own header file include */

/* system library header file includes */
#include <stdint.h>

/* other library header file includes */
#include <pt/timer.h>

/* own component header file includes */
#include <rv-link/link/led.h>


uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp)
{
    struct timer timer;

    timer_set(&timer, 100 * 1000 /* 100 ms */);

    for(;;) {
        rvl_led_assert(1);
        while (!timer_expired(&timer));
        timer_reset(&timer);

        rvl_led_assert(0);
        while (!timer_expired(&timer));
        timer_reset(&timer);
    }

    return 0;
}

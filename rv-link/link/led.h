#ifndef __RV_LINK_LINK_LED_H__
#define __RV_LINK_LINK_LED_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/led.h
 * \brief RV-Link LED interface
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

typedef enum rvl_led_indicator_enum {
    RVL_LED_INDICATOR_GDB_CONNECT,
    RVL_LED_INDICATOR_TARGET_RUN,
    RVL_LED_INDICATOR_ASSERT,
    RVL_LED_INDICATOR_LINK_USB_TX,
    RVL_LED_INDICATOR_LINK_USB_RX,
    RVL_LED_INDICATOR_LINK_SER_TX,
    RVL_LED_INDICATOR_LINK_SER_RX,
    RVL_LED_INDICATOR_LINK_CAN_TX,
    RVL_LED_INDICATOR_LINK_CAN_RX,
    RVL_LED_INDICATOR_MAX,
} rvl_led_indicator_enum_t;


void rvl_led_init(void);
PT_THREAD(rvl_led_poll(void));

void rvl_led_gdb_connect(int on);
void rvl_led_link_run(int on);
void rvl_led_target_run(int on);
void rvl_led_assert(int on);

/* Communication indicator LEDs */
#ifdef RVL_LINK_RVL_PROBE
void rvl_led_indicator(rvl_led_indicator_enum_t ind, int on);
#else
#define rvl_led_indicator(ind, on) ((void)0)
#endif

#endif /* __RV_LINK_LINK_LED_H__ */

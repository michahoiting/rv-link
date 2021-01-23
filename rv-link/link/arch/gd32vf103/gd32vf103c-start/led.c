/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2021, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103/rvl-probe/led.c
 * \brief Led handling for the GD32VF103C-START board.
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
#include <rv-link/link/led.h>

/* system library header file includes */
#include <stdbool.h>
#include <stdint.h>

/* other library header file includes */
#include <pt/pt.h>
#include <pt/timer.h>
#include "nuclei_sdk_soc.h"
#include "riscv_encoding.h"

typedef struct rvl_led_s
{
    struct pt pt;
    bool gdb_connect;
    bool target_run;
    uint32_t on_period;
    uint32_t off_period;
    struct timer timer;
} rvl_led_t;

static rvl_led_t rvl_led_i;
#define self rvl_led_i

#define LED_GDB_CONNECT_PORT    GPIOA
#define LED_GDB_CONNECT_PIN     GPIO_PIN_1

#define LED1_PORT               GPIOA
#define LED1_PIN                GPIO_PIN_7


void rvl_led_init(void)
{
    PT_INIT(&self.pt);
    self.gdb_connect = false;
    self.target_run = false;

    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(LED_GDB_CONNECT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_GDB_CONNECT_PIN);
    gpio_bit_reset(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);

    gpio_init(LED1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED1_PIN);
    gpio_bit_reset(LED1_PORT, LED1_PIN);
}


PT_THREAD(rvl_led_poll(void))
{
    PT_BEGIN(&self.pt);

    if (self.gdb_connect) {
        if (self.target_run) {
            self.on_period = 10 * 1000 * 1000;
            self.off_period = 10 * 1000 * 1000;
        } else {
            self.on_period = 50 * 1000 * 1000;
            self.off_period = 50 * 1000 * 1000;
        }
    } else {
        self.on_period = 10 * 1000 * 1000;
        self.off_period = 100 * 1000 * 1000;
    }

    gpio_bit_set(LED1_PORT, LED1_PIN);
    timer_set(&self.timer, self.on_period);
    PT_WAIT_UNTIL(&self.pt, timer_expired(&self.timer));

    gpio_bit_reset(LED1_PORT, LED1_PIN);
    timer_set(&self.timer, self.off_period);
    PT_WAIT_UNTIL(&self.pt, timer_expired(&self.timer));

    PT_END(&self.pt);
}


void rvl_led_gdb_connect(int connect)
{
    if (connect) {
        gpio_bit_set(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    } else {
        gpio_bit_reset(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    }

    self.gdb_connect = (bool)connect;
}


void rvl_led_link_run(int on)
{
    if (on) {
        gpio_bit_set(LED1_PORT, LED1_PIN);
    } else {
        gpio_bit_reset(LED1_PORT, LED1_PIN);
    }
}


void rvl_led_target_run(int on)
{
    self.target_run = (bool)on;
}


void rvl_led_assert(int on)
{
    if (on) {
        gpio_bit_set(LED1_PORT, LED1_PIN);
    } else {
        gpio_bit_reset(LED1_PORT, LED1_PIN);
    }
}

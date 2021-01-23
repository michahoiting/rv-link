/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103/rvl-probe/led.c
 * \brief Led handling for the RVL-Probe board.
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

/* RV-Link assert led */
#define LED_RGB_RED_PORT            GPIOC
#define LED_RGB_RED_PIN             GPIO_PIN_13
#define LED_RGB_RED_RCU             RCU_GPIOC

/* GDB connect led */
#define LED_RGB_GREEN_PORT          GPIOA
#define LED_RGB_GREEN_PIN           GPIO_PIN_1
#define LED_RGB_GREEN_RCU           RCU_GPIOA

/* unused RGB sub-led */
#define LED_RGB_BLUE_PORT           GPIOA
#define LED_RGB_BLUE_PIN            GPIO_PIN_2
#define LED_RGB_BLUE_RCU            RCU_GPIOA

/* USB communication indicator led */
#define LED_USB_COM_PORT            GPIOB
#define LED_USB_COM_PIN             GPIO_PIN_4
#define LED_USB_COM_RCU             RCU_GPIOB

/* Target CAN bus communication indicator led */
#define LED_CAN_COM_PORT            GPIOB
#define LED_CAN_COM_PIN             GPIO_PIN_5
#define LED_CAN_COM_RCU             RCU_GPIOB

/* Target serial communication RX/TX indicator leds */
#define LED_SER_RX_PORT             GPIOB
#define LED_SER_RX_PIN              GPIO_PIN_6
#define LED_SER_RX_RCU              RCU_GPIOB

#define LED_SER_TX_PORT             GPIOB
#define LED_SER_TX_PIN              GPIO_PIN_7
#define LED_SER_TX_RCU              RCU_GPIOB


typedef struct rvl_led_state
{
    struct pt pt;
    bool on;
    uint32_t on_period;
    uint32_t off_period;
    struct timer timer;
} rvl_led_state_t;

typedef struct rvl_led
{
    struct pt pt;
    rvl_led_state_t led_state[RVL_LED_INDICATOR_MAX];
} rvl_led_t;

static rvl_led_t rvl_led_i;
#define self rvl_led_i

const static struct led_config {
    uint32_t port;
    uint32_t pin;
    rcu_periph_enum rcu;
} LED[] = {

    /* GDB connected */
    {LED_RGB_GREEN_PORT, LED_RGB_GREEN_PIN, LED_RGB_GREEN_RCU},

    /* Target running (also mapped onto the GDB connected led) */
    {LED_RGB_GREEN_PORT, LED_RGB_GREEN_PIN, LED_RGB_GREEN_RCU},

    /* RVL-Link internal failure */
    {LED_RGB_RED_PORT, LED_RGB_RED_PIN, LED_RGB_RED_RCU},

    /* USB communication */
    {LED_USB_COM_PORT, LED_USB_COM_PIN, LED_USB_COM_RCU},

    /* Target CAN bus communication */
    {LED_CAN_COM_PORT, LED_CAN_COM_PIN, LED_CAN_COM_RCU},

    /* Target serial communication Tx/Rx */
    {LED_SER_RX_PORT, LED_SER_RX_PIN, LED_SER_RX_RCU},
    {LED_SER_TX_PORT, LED_SER_TX_PIN, LED_SER_TX_RCU},

    /* Unused led */
    {LED_RGB_BLUE_PORT, LED_RGB_BLUE_PIN, LED_RGB_BLUE_RCU},
};

static void rvl_led_init_gpio(uint32_t port, uint32_t pin, rcu_periph_enum rcu);
static PT_THREAD(rvl_led_gdb_connect_poll(rvl_led_indicator_enum_t ind, rvl_led_state_t* led_state));
static PT_THREAD(rvl_led_indicator_poll(rvl_led_indicator_enum_t ind, rvl_led_state_t* led_state));


void rvl_led_init(void)
{
    int i;
    PT_INIT(&self.pt);

    for (i=0; i < RVL_LED_INDICATOR_MAX; i++) {
        PT_INIT(&self.led_state[i].pt);
        self.led_state[i].on = false;
    }

    /* Initialize the leds */
    for (i=0; i < sizeof(LED)/sizeof(struct led_config); i++) {
        rvl_led_init_gpio(LED[i].port, LED[i].pin, LED[i].rcu);
    }
}


PT_THREAD(rvl_led_poll(void))
{
    rvl_led_indicator_enum_t i;

    PT_BEGIN(&self.pt);

    for (i=0; i < RVL_LED_INDICATOR_MAX; i++) {
       (void) PT_SCHEDULE(rvl_led_indicator_poll(i,  &self.led_state[i]));
    }

    PT_END(&self.pt);
}


void rvl_led_gdb_connect(int connect)
{
    self.led_state[RVL_LED_INDICATOR_GDB_CONNECT].on = connect;
}


void rvl_led_link_run(int on)
{
}


void rvl_led_target_run(int on)
{
    self.led_state[RVL_LED_INDICATOR_TARGET_RUN].on = on;
}


void rvl_led_assert(int on)
{
    self.led_state[RVL_LED_INDICATOR_ASSERT].on = on;
    if (on) {
        gpio_bit_reset(LED[RVL_LED_INDICATOR_ASSERT].port, LED[RVL_LED_INDICATOR_ASSERT].pin);
    } else {
        gpio_bit_set(LED[RVL_LED_INDICATOR_ASSERT].port, LED[RVL_LED_INDICATOR_ASSERT].pin);
    }
}


void rvl_led_indicator(rvl_led_indicator_enum_t ind, int on)
{
    self.led_state[ind].on = on;
}


static void rvl_led_init_gpio(uint32_t port, uint32_t pin, rcu_periph_enum rcu)
{
    rcu_periph_clock_enable(rcu);
    gpio_init(port, GPIO_MODE_OUT_OD, GPIO_OSPEED_2MHZ, pin);
    gpio_bit_set(port, pin);
}


static PT_THREAD(rvl_led_indicator_poll(rvl_led_indicator_enum_t ind, rvl_led_state_t* led_state))
{
    /* The GDB connected indicator has its own handler */
    if (ind == RVL_LED_INDICATOR_GDB_CONNECT) {
        return PT_SCHEDULE(rvl_led_gdb_connect_poll(ind, led_state));
    }

    /* Skip indicators with no handling */
    if (ind == RVL_LED_INDICATOR_TARGET_RUN ||
        ind == RVL_LED_INDICATOR_ASSERT) {
        return PT_ENDED;
    }

    PT_BEGIN(&led_state->pt);

    PT_WAIT_UNTIL(&led_state->pt, led_state->on);
    led_state->on = false;

    /* pulse led */
#define RVL_LED_INDICATOR_PULSE_ON_PERIOD  (200 * 1000)
#define RVL_LED_INDICATOR_PULSE_OFF_PERIOD (100 * 1000)

    gpio_bit_reset(LED[ind].port, LED[ind].pin);
    timer_set(&led_state->timer, RVL_LED_INDICATOR_PULSE_ON_PERIOD);
    PT_WAIT_UNTIL(&led_state->pt, timer_expired(&led_state->timer));

    gpio_bit_set(LED[ind].port, LED[ind].pin);
    timer_set(&led_state->timer, RVL_LED_INDICATOR_PULSE_OFF_PERIOD);
    PT_WAIT_UNTIL(&led_state->pt, timer_expired(&led_state->timer));

    PT_END(&led_state->pt);
}


static PT_THREAD(rvl_led_gdb_connect_poll(rvl_led_indicator_enum_t ind, rvl_led_state_t* led_state))
{
    PT_BEGIN(&led_state->pt);

    /* The gdb server status blink pattern depends on the target_run state */
    if (led_state->on) {
        if (self.led_state[RVL_LED_INDICATOR_TARGET_RUN].on) {
            led_state->on_period = 100 * 1000;
            led_state->off_period = 100 * 1000;
        } else {
            led_state->on_period = 500 * 1000;
            led_state->off_period = 500 * 1000;
        }
    } else {
        led_state->on_period = 100 * 1000;
        led_state->off_period = 1000 * 1000;
    }

    gpio_bit_reset(LED[RVL_LED_INDICATOR_GDB_CONNECT].port, LED[RVL_LED_INDICATOR_GDB_CONNECT].pin);

    timer_set(&led_state->timer, led_state->on_period);
    PT_WAIT_UNTIL(&led_state->pt, timer_expired(&led_state->timer));

    gpio_bit_set(LED[RVL_LED_INDICATOR_GDB_CONNECT].port, LED[RVL_LED_INDICATOR_GDB_CONNECT].pin);

    timer_set(&led_state->timer, led_state->off_period);
    PT_WAIT_UNTIL(&led_state->pt, timer_expired(&led_state->timer));

    PT_END(&led_state->pt);
}

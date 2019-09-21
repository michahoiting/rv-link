#include "gd32vf103.h"
#include "riscv_encoding.h"
#include "rvl-led.h"

#define LED_GDB_CONNECT_PORT    GPIOA
#define LED_GDB_CONNECT_PIN     GPIO_PIN_2

#define LED_TARGET_RUN_PORT     GPIOA
#define LED_TARGET_RUN_PIN      GPIO_PIN_1

#define LED_ASSERT_PORT         GPIOC
#define LED_ASSERT_PIN          GPIO_PIN_13


void rvl_led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(LED_GDB_CONNECT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_GDB_CONNECT_PIN);
    gpio_bit_set(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);

    gpio_init(LED_TARGET_RUN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_TARGET_RUN_PIN);
    gpio_bit_set(LED_TARGET_RUN_PORT, LED_TARGET_RUN_PIN);

    gpio_init(LED_ASSERT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_ASSERT_PIN);
    gpio_bit_set(LED_ASSERT_PORT, LED_ASSERT_PIN);
}


void rvl_led_gdb_connect(int connect)
{
    if (!connect) {
        gpio_bit_set(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    } else {
        gpio_bit_reset(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    }
}


void rvl_led_target_run(int on)
{
    if (!on) {
        gpio_bit_set(LED_TARGET_RUN_PORT, LED_TARGET_RUN_PIN);
    } else {
        gpio_bit_reset(LED_TARGET_RUN_PORT, LED_TARGET_RUN_PIN);
    }
}


void rvl_led_assert(int on)
{
    if (!on) {
        gpio_bit_set(LED_ASSERT_PORT, LED_ASSERT_PIN);
    } else {
        gpio_bit_reset(LED_ASSERT_PORT, LED_ASSERT_PIN);
    }
}

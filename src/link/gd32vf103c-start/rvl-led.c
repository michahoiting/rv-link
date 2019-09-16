#include "gd32vf103.h"
#include "riscv_encoding.h"
#include "led.h"

#define LED_GDB_CONNECT_PORT    GPIOA
#define LED_GDB_CONNECT_PIN     GPIO_PIN_1

#define LED_LINK_RUN_PORT       GPIOA
#define LED_LINK_RUN_PIN        GPIO_PIN_7


void rvl_led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(LED_GDB_CONNECT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_GDB_CONNECT_PIN);
    gpio_bit_reset(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);

    gpio_init(LED_LINK_RUN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_LINK_RUN_PIN);
    gpio_bit_reset(LED_LINK_RUN_PORT, LED_LINK_RUN_PIN);
}


void rvl_led_gdb_connect(int connect)
{
    if (connect) {
        gpio_bit_set(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    } else {
        gpio_bit_reset(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    }
}


void rvl_led_link_run(int on)
{
    if (on) {
        gpio_bit_set(LED_LINK_RUN_PORT, LED_LINK_RUN_PIN);
    } else {
        gpio_bit_reset(LED_LINK_RUN_PORT, LED_LINK_RUN_PIN);
    }
}

#include <stdbool.h>
#include <stdint.h>

#include "gd32vf103.h"
#include "riscv_encoding.h"
#include "rvl-led.h"
#include "pt/pt.h"


typedef struct rvl_led_s
{
    struct pt pt;
    bool gdb_connect;
    bool target_run;
    uint32_t on_period;
    uint32_t off_period;
    uint32_t mcycle_start;
}rvl_led_t;

static rvl_led_t rvl_led_i;
#define self rvl_led_i


#define LED_BLUE_PORT           GPIOA
#define LED_BLUE_PIN            GPIO_PIN_2

#define LED_GREEN_PORT          GPIOA
#define LED_GREEN_PIN           GPIO_PIN_1

#define LED_RED_PORT            GPIOC
#define LED_RED_PIN             GPIO_PIN_13


void rvl_led_init(void)
{
    PT_INIT(&self.pt);
    self.gdb_connect = FALSE;
    self.target_run = FALSE;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(LED_BLUE_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_BLUE_PIN);
    gpio_bit_set(LED_BLUE_PORT, LED_BLUE_PIN);

    gpio_init(LED_GREEN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_GREEN_PIN);
    gpio_bit_set(LED_GREEN_PORT, LED_GREEN_PIN);

    gpio_init(LED_RED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_RED_PIN);
    gpio_bit_set(LED_RED_PORT, LED_RED_PIN);
}


PT_THREAD(rvl_led_poll(void))
{
    PT_BEGIN(&self.pt);

    if(self.gdb_connect) {
        if(self.target_run) {
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

    gpio_bit_reset(LED_GREEN_PORT, LED_GREEN_PIN);
    self.mcycle_start = read_csr(mcycle);
    PT_WAIT_UNTIL(&self.pt, read_csr(mcycle) - self.mcycle_start >= self.on_period);

    gpio_bit_set(LED_GREEN_PORT, LED_GREEN_PIN);
    self.mcycle_start = read_csr(mcycle);
    PT_WAIT_UNTIL(&self.pt, read_csr(mcycle) - self.mcycle_start >= self.off_period);

    PT_END(&self.pt);
}


void rvl_led_gdb_connect(int connect)
{
    self.gdb_connect = (bool)connect;
}


void rvl_led_link_run(int on)
{
    if (!on) {
        gpio_bit_set(LED_GREEN_PORT, LED_GREEN_PIN);
    } else {
        gpio_bit_reset(LED_GREEN_PORT, LED_GREEN_PIN);
    }
}


void rvl_led_target_run(int on)
{
    self.target_run = (bool)on;
}


void rvl_led_assert(int on)
{
    if (!on) {
        gpio_bit_set(LED_RED_PORT, LED_RED_PIN);
    } else {
        gpio_bit_reset(LED_RED_PORT, LED_RED_PIN);
    }
}

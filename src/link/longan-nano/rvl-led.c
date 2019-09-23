#include "gd32vf103.h"
#include "riscv_encoding.h"
#include "rvl-led.h"
#include "pt.h"


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


#define LED_GDB_CONNECT_PORT    GPIOA
#define LED_GDB_CONNECT_PIN     GPIO_PIN_2

#define LED_TARGET_RUN_PORT     GPIOA
#define LED_TARGET_RUN_PIN      GPIO_PIN_1

#define LED_ASSERT_PORT         GPIOC
#define LED_ASSERT_PIN          GPIO_PIN_13


void rvl_led_init(void)
{
    PT_INIT(&self.pt);
    self.gdb_connect = FALSE;
    self.target_run = FALSE;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(LED_GDB_CONNECT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_GDB_CONNECT_PIN);
    gpio_bit_set(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);

    gpio_init(LED_TARGET_RUN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_TARGET_RUN_PIN);
    gpio_bit_set(LED_TARGET_RUN_PORT, LED_TARGET_RUN_PIN);

    gpio_init(LED_ASSERT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LED_ASSERT_PIN);
    gpio_bit_set(LED_ASSERT_PORT, LED_ASSERT_PIN);
}


#define CYCLES_PER_POLL     20
PT_THREAD(rvl_led_poll(void))
{
    PT_BEGIN(&self.pt);

    if(self.target_run) {
        self.on_period = 10 * 1000 * 1000 / CYCLES_PER_POLL;
        self.off_period = 10 * 1000 * 1000 / CYCLES_PER_POLL;
    } else if(self.gdb_connect) {
        self.on_period = 50 * 1000 * 1000 / CYCLES_PER_POLL;
        self.off_period = 50 * 1000 * 1000 / CYCLES_PER_POLL;
    } else {
        self.on_period = 10 * 1000 * 1000 / CYCLES_PER_POLL;
        self.off_period = 100 * 1000 * 1000 / CYCLES_PER_POLL;
    }

    gpio_bit_set(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    self.mcycle_start = read_csr(mcycle);
    PT_WAIT_UNTIL(&self.pt, read_csr(mcycle) - self.mcycle_start >= self.on_period);

    gpio_bit_reset(LED_GDB_CONNECT_PORT, LED_GDB_CONNECT_PIN);
    self.mcycle_start = read_csr(mcycle);
    PT_WAIT_UNTIL(&self.pt, read_csr(mcycle) - self.mcycle_start >= self.off_period);

    PT_END(&self.pt);
}


void rvl_led_gdb_connect(int connect)
{
    self.gdb_connect = (bool)connect;
}


void rvl_led_target_run(int on)
{
    if (!on) {
        gpio_bit_set(LED_TARGET_RUN_PORT, LED_TARGET_RUN_PIN);
    } else {
        gpio_bit_reset(LED_TARGET_RUN_PORT, LED_TARGET_RUN_PIN);
    }

    self.target_run = (bool)on;
}


void rvl_led_assert(int on)
{
    if (!on) {
        gpio_bit_set(LED_ASSERT_PORT, LED_ASSERT_PIN);
    } else {
        gpio_bit_reset(LED_ASSERT_PORT, LED_ASSERT_PIN);
    }
}

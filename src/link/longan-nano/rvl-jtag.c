#include "gd32vf103.h"
#include "riscv_encoding.h"
#include "rvl-jtag.h"
#include "rvl-link-config.h"

#ifdef RVL_LINK_LONGAN_NANO_JTAG_PIN_ALT

#define TCK_PORT    GPIOB
#define TCK_PIN     GPIO_PIN_9

#define TMS_PORT    GPIOB
#define TMS_PIN     GPIO_PIN_8

#define TDI_PORT    GPIOB
#define TDI_PIN     GPIO_PIN_7

#define TDO_PORT    GPIOB
#define TDO_PIN     GPIO_PIN_6

#define SRST_PORT   GPIOB
#define SRST_PIN    GPIO_PIN_5

#else

#define TCK_PORT    GPIOA
#define TCK_PIN     GPIO_PIN_14

#define TMS_PORT    GPIOA
#define TMS_PIN     GPIO_PIN_13

#define TDI_PORT    GPIOA
#define TDI_PIN     GPIO_PIN_15

#define TDO_PORT    GPIOB
#define TDO_PIN     GPIO_PIN_3

#define SRST_PORT   GPIOA
#define SRST_PIN    GPIO_PIN_10

#endif


void rvl_jtag_init(void)
{
#ifdef RVL_LINK_LONGAN_NANO_JTAG_PIN_ALT
    rcu_periph_clock_enable(RCU_GPIOB);
#else
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);
#endif

    gpio_init(TCK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, TCK_PIN);
    gpio_init(TMS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, TMS_PIN);
    gpio_init(TDI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, TDI_PIN);
    gpio_init(TDO_PORT, GPIO_MODE_IPU, 0, TDO_PIN);
    gpio_init(SRST_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ, SRST_PIN);
}


void rvl_jtag_fini(void)
{
    gpio_init(TCK_PORT, GPIO_MODE_AIN, 0, TCK_PIN);
    gpio_init(TMS_PORT, GPIO_MODE_AIN, 0, TMS_PIN);
    gpio_init(TDI_PORT, GPIO_MODE_AIN, 0, TDI_PIN);
    gpio_init(TDO_PORT, GPIO_MODE_AIN, 0, TDO_PIN);
    gpio_init(SRST_PORT, GPIO_MODE_AIN, 0, SRST_PIN);
}


void rvl_jtag_tms_put(int tms)
{
    if (tms) {
        gpio_bit_set(TMS_PORT, TMS_PIN);
    } else {
        gpio_bit_reset(TMS_PORT, TMS_PIN);
    }
}


void rvl_jtag_tdi_put(int tdi)
{
    if (tdi) {
        gpio_bit_set(TDI_PORT, TDI_PIN);
    } else {
        gpio_bit_reset(TDI_PORT, TDI_PIN);
    }
}


void rvl_jtag_tck_put(int tck)
{
    if (tck) {
        gpio_bit_set(TCK_PORT, TCK_PIN);
    } else {
        gpio_bit_reset(TCK_PORT, TCK_PIN);
    }
}


void rvl_jtag_delay_half_period()
{
	uint32_t start = read_csr(mcycle);

	while(read_csr(mcycle) - start < 100);
}


int rvl_jtag_tdo_get()
{
    int tdo;

    if (gpio_input_bit_get(TDO_PORT, TDO_PIN)) {
        tdo = 1;
    } else {
        tdo = 0;
    }

    return tdo;
}


void rvl_jtag_srst_put(int srst)
{
    if (srst) {
        gpio_bit_set(SRST_PORT, SRST_PIN);
    } else {
        gpio_bit_reset(SRST_PORT, SRST_PIN);
    }
}


void rvl_jtag_delay(uint32_t us)
{
    uint32_t start = read_csr(mcycle);
    uint32_t period = us * 96; // CPU run @96MHz

    while(read_csr(mcycle) - start < period);
}

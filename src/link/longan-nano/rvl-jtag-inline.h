#ifndef __RVL_JTAG_INLINE_H__
#define __RVL_JTAG_INLINE_H__

#include <stdint.h>

#include "riscv_encoding.h"
#include "gd32vf103_gpio.h"
#include "rvl-link-config.h"


#ifdef RVL_LINK_CONFIG_JTAG_PIN_ALT

// PB13, SCK
#define TCK_PORT    GPIOB
#define TCK_PIN     GPIO_PIN_13

// PB12, CS
#define TMS_PORT    GPIOB
#define TMS_PIN     GPIO_PIN_12

// PB15, MOSI
#define TDI_PORT    GPIOB
#define TDI_PIN     GPIO_PIN_15

// PB14, MISO
#define TDO_PORT    GPIOB
#define TDO_PIN     GPIO_PIN_14

// PA8
#define SRST_PORT   GPIOA
#define SRST_PIN    GPIO_PIN_8

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
#define SRST_PIN    GPIO_PIN_9

#endif


static inline void rvl_jtag_tms_put(int tms)
{
    if (tms) {
        GPIO_BOP(TMS_PORT) = (uint32_t)TMS_PIN;
    } else {
        GPIO_BC(TMS_PORT) = (uint32_t)TMS_PIN;
    }
}


static inline void rvl_jtag_tdi_put(int tdi)
{
    if (tdi) {
        GPIO_BOP(TDI_PORT) = (uint32_t)TDI_PIN;
    } else {
        GPIO_BC(TDI_PORT) = (uint32_t)TDI_PIN;
    }
}


static inline void rvl_jtag_tck_put(int tck)
{
    if (tck) {
        GPIO_BOP(TCK_PORT) = (uint32_t)TCK_PIN;
    } else {
        GPIO_BC(TCK_PORT) = (uint32_t)TCK_PIN;
    }
}


#ifndef RVL_JTAG_TCK_FREQ_KHZ
#define RVL_JTAG_TCK_FREQ_KHZ       1000
#endif

#if RVL_JTAG_TCK_FREQ_KHZ >= 1000

#define RVL_JTAG_TCK_HIGH_DELAY     30
#define RVL_JTAG_TCK_LOW_DELAY      1

#elif RVL_JTAG_TCK_FREQ_KHZ >= 500

#define RVL_JTAG_TCK_HIGH_DELAY     (30 + 50)
#define RVL_JTAG_TCK_LOW_DELAY      (1 + 50)

#elif RVL_JTAG_TCK_FREQ_KHZ >= 200

#define RVL_JTAG_TCK_HIGH_DELAY     (30 + 200)
#define RVL_JTAG_TCK_LOW_DELAY      (1 + 200)

#else // 100KHz

#define RVL_JTAG_TCK_HIGH_DELAY     (30 + 450)
#define RVL_JTAG_TCK_LOW_DELAY      (1 + 450)

#endif

static inline void rvl_jtag_tck_high_delay()
{
    uint32_t start = read_csr(mcycle);

    while(read_csr(mcycle) - start < RVL_JTAG_TCK_HIGH_DELAY);
}


static inline void rvl_jtag_tck_low_delay()
{
    uint32_t start = read_csr(mcycle);

    while(read_csr(mcycle) - start < RVL_JTAG_TCK_LOW_DELAY);
}


static inline int rvl_jtag_tdo_get()
{
    if ((uint32_t) RESET != (GPIO_ISTAT(TDO_PORT) & (TDO_PIN))) {
        return 1;
    } else {
        return 0;
    }
}


#endif /* __RVL_JTAG_INLINE_H__ */

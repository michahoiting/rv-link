#ifndef __RV_LINK_LINK_ARCH_GD32VF103C_LONGAN_NANO_JTAG_INLINE_H__
#define __RV_LINK_LINK_ARCH_GD32VF103C_LONGAN_NANO_JTAG_INLINE_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/longan-nano/jtag-inline.h
 * \brief TODO
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

/* system library header file includes */
#include <stdint.h>

/* other project header file includes */
#include <gd32vf103-sdk/RISCV/drivers/riscv_encoding.h>
#include <gd32vf103-sdk/GD32VF103_standard_peripheral/Include/gd32vf103_gpio.h>


#define TCK_PORT    GPIOA
#define TCK_PIN     GPIO_PIN_14

#define TMS_PORT    GPIOA
#define TMS_PIN     GPIO_PIN_13

#define TDI_PORT    GPIOA
#define TDI_PIN     GPIO_PIN_15

#define TDO_PORT    GPIOB
#define TDO_PIN     GPIO_PIN_3


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

    while (read_csr(mcycle) - start < RVL_JTAG_TCK_HIGH_DELAY) ;
}


static inline void rvl_jtag_tck_low_delay()
{
    uint32_t start = read_csr(mcycle);

    while (read_csr(mcycle) - start < RVL_JTAG_TCK_LOW_DELAY) ;
}


static inline int rvl_jtag_tdo_get()
{
    if ((uint32_t) RESET != (GPIO_ISTAT(TDO_PORT) & (TDO_PIN))) {
        return 1;
    } else {
        return 0;
    }
}

#endif /* __RV_LINK_LINK_ARCH_GD32VF103C_LONGAN_NANO_JTAG_INLINE_H__ */

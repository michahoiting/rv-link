/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103/longan-nano/jtag.c
 * \brief JTAG functions specifically for the Longan Nano board.
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
#include <rv-link/link/jtag.h>

/* other library header file includes */
#include "gd32vf103_soc_sdk.h"

/* own component header file includes */
#include <rv-link/link/arch/gd32vf103/longan-nano/link-config.h>


void rvl_jtag_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    // TODO verify line below
    AFIO_PCF0 = (AFIO_PCF0 & 0xF8FFFFFF) | 0x04000000;

    gpio_init(RVL_LINK_TCK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, RVL_LINK_TCK_PIN);
    gpio_init(RVL_LINK_TMS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, RVL_LINK_TMS_PIN);
    gpio_init(RVL_LINK_TDI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, RVL_LINK_TDI_PIN);
    gpio_init(RVL_LINK_TDO_PORT, GPIO_MODE_IPU, 0, RVL_LINK_TDO_PIN);
}


void rvl_jtag_fini(void)
{
    gpio_init(RVL_LINK_TCK_PORT, GPIO_MODE_AIN, 0, RVL_LINK_TCK_PIN);
    gpio_init(RVL_LINK_TMS_PORT, GPIO_MODE_AIN, 0, RVL_LINK_TMS_PIN);
    gpio_init(RVL_LINK_TDI_PORT, GPIO_MODE_AIN, 0, RVL_LINK_TDI_PIN);
    gpio_init(RVL_LINK_TDO_PORT, GPIO_MODE_AIN, 0, RVL_LINK_TDO_PIN);
}


void rvl_jtag_srst_put(int srst)
{
#ifdef RVL_LINK_SRST_PORT
    if (srst) {
        gpio_init(RVL_LINK_SRST_PORT, GPIO_MODE_AIN, 0, RVL_LINK_SRST_PIN);
    } else {
        gpio_bit_reset(RVL_LINK_SRST_PORT, RVL_LINK_SRST_PIN);
        gpio_init(RVL_LINK_SRST_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ, RVL_LINK_SRST_PIN);
    }
#endif
}

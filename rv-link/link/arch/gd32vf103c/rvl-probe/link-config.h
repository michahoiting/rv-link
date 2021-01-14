#ifndef __RV_LINK_LINK_ARCH_GD32VF103C_RVL_PROBE_LINK_CONFIG_H__
#define __RV_LINK_LINK_ARCH_GD32VF103C_RVL_PROBE_LINK_CONFIG_H__
/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/rvl-probe/link-config.h
 * \brief Link configuration header file for the Longan Nano board with
 *        alternative pin usage.
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

/* other library header file includes */
#include <gd32vf103-sdk/GD32VF103_standard_peripheral/gd32vf103.h>

#define RVL_LINK_CONFIG_NAME                "RVL-Probe"

#if defined(RVL_PROBE_LINK_CONFIG_SPI1_NO_REMAP)
    /* JTAG TMS pin definition */
    #define RVL_LINK_TMS_PORT                   GPIOB
    #define RVL_LINK_TMS_PIN                    GPIO_PIN_12 /* PB12, NSS1 */
    #define RVL_LINK_TMS_CLK                    RCU_GPIOB

    /* JTAG TCK pin definition */
    #define RVL_LINK_TCK_PORT                   GPIOB
    #define RVL_LINK_TCK_PIN                    GPIO_PIN_13 /* PB13, SCK1 */
    #define RVL_LINK_TCK_CLK                    RCU_GPIOB

    /* JTAG TDO pin definition */
    #define RVL_LINK_TDO_PORT                   GPIOB
    #define RVL_LINK_TDO_PIN                    GPIO_PIN_14 /* PB14, MISO1 */
    #define RVL_LINK_TDO_CLK                    RCU_GPIOB

    /* JTAG TDI pin definition */
    #define RVL_LINK_TDI_PORT                   GPIOB
    #define RVL_LINK_TDI_PIN                    GPIO_PIN_15 /* PB15, MOSI1 */
    #define RVL_LINK_TDI_CLK                    RCU_GPIOB

    /* JTAG SRST pin definition */
    #define RVL_LINK_SRST_PORT                  GPIOA
    #define RVL_LINK_SRST_PIN                   GPIO_PIN_8 /* PA8 */
    #define RVL_LINK_SRST_CLK                   RCU_GPIOA
#elif defined(RVL_PROBE_LINK_CONFIG_SPI0_NO_REMAP)
    /* JTAG TMS pin definition */
    #define RVL_LINK_TMS_PORT                   GPIOA
    #define RVL_LINK_TMS_PIN                    GPIO_PIN_4 /* PA4, NSS0 */
    #define RVL_LINK_TMS_CLK                    RCU_GPIOA

    /* JTAG TCK pin definition */
    #define RVL_LINK_TCK_PORT                   GPIOA
    #define RVL_LINK_TCK_PIN                    GPIO_PIN_5 /* PA5, SCK0 */
    #define RVL_LINK_TCK_CLK                    RCU_GPIOA

    /* JTAG TDO pin definition */
    #define RVL_LINK_TDO_PORT                   GPIOA
    #define RVL_LINK_TDO_PIN                    GPIO_PIN_6 /* PA6, MISO0 */
    #define RVL_LINK_TDO_CLK                    RCU_GPIOA

    /* JTAG TDI pin definition */
    #define RVL_LINK_TDI_PORT                   GPIOA
    #define RVL_LINK_TDI_PIN                    GPIO_PIN_7 /* PA7, MOSI0 */
    #define RVL_LINK_TDI_CLK                    RCU_GPIOA

    /* NO JTAG SRST pin definition */
    #undef RVL_LINK_SRST_PORT
    #undef RVL_LINK_SRST_PIN
    #undef RVL_LINK_SRST_CLK
#else
    #error No RVL_PROBE_LINK_CONFIG_SPIx defined
#endif
#endif /* __RV_LINK_LINK_ARCH_GD32VF103C_RVL_PROBE_LINK_CONFIG_H__ */

#ifndef __RV_LINK_LINK_ARCH_GD32VF103C_GD32VF103C_START_CONFIG_H__
#define __RV_LINK_LINK_ARCH_GD32VF103C_GD32VF103C_START_CONFIG_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/longan-nano/link-config.h
 * \brief Configuration header file for the Longan Nano board
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

/* other project header file includes */
#include <gd32vf103-sdk/GD32VF103_standard_peripheral/Include/gd32vf103_gpio.h>


/* JTAG TCK pin definition */
#define RVL_LINK_TCK_PORT                   GPIOB
#define RVL_LINK_TCK_PIN                    GPIO_PIN_13 /* PB13, SCK1 */

/* JTAG TMS pin definition */
#define RVL_LINK_TMS_PORT                   GPIOB
#define RVL_LINK_TMS_PIN                    GPIO_PIN_12 /* PB12, NSS1 */

/* JTAG TDI pin definition */
#define RVL_LINK_TDI_PORT                   GPIOB
#define RVL_LINK_TDI_PIN                    GPIO_PIN_15 /* PB15, MOSI1 */

/* JTAG TDO pin definition */
#define RVL_LINK_TDO_PORT                   GPIOB
#define RVL_LINK_TDO_PIN                    GPIO_PIN_14 /* PB14, MISO1 */

/* JTAG SRST pin definition */
#define RVL_LINK_SRST_PORT                  GPIOB
#define RVL_LINK_SRST_PIN                   GPIO_PIN_0 /* PB0 */

#define RVL_LINK_CONFIG_NAME                "GD32VF103C-START"

#endif /* __RV_LINK_LINK_ARCH_GD32VF103C_GD32VF103C_START_CONFIG_H__ */

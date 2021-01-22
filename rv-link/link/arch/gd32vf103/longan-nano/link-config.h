#ifndef __RV_LINK_LINK_ARCH_GD32VF103C_LONGAN_NANO_LINK_CONFIG_H__
#define __RV_LINK_LINK_ARCH_GD32VF103C_LONGAN_NANO_LINK_CONFIG_H__
/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103/longan-nano/link-config.h
 * \brief Link configuration header file for the Longan Nano board.
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
#include "nuclei_sdk_soc.h"

#define RVL_LINK_CONFIG_NAME                "Longan Nano"

/* JTAG TCK pin definition */
#define RVL_LINK_TCK_PORT                   GPIOA
#define RVL_LINK_TCK_PIN                    GPIO_PIN_14 /* PA14, JTCK */

/* JTAG TMS pin definition */
#define RVL_LINK_TMS_PORT                   GPIOA
#define RVL_LINK_TMS_PIN                    GPIO_PIN_13 /* PA13, JTMS */

/* JTAG TDI pin definition */
#define RVL_LINK_TDI_PORT                   GPIOA
#define RVL_LINK_TDI_PIN                    GPIO_PIN_15 /* PA15, JTDI */

/* JTAG TDO pin definition */
#define RVL_LINK_TDO_PORT                   GPIOB
#define RVL_LINK_TDO_PIN                    GPIO_PIN_3 /* PB3, JTDO */

/* JTAG SRST pin definition */
#undef RVL_LINK_SRST_PORT
#undef RVL_LINK_SRST_PIN                    /* No JTAG SRST pin defined */

#endif /* __RV_LINK_LINK_ARCH_GD32VF103C_LONGAN_NANO_LINK_CONFIG_H__ */

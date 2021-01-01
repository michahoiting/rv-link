#ifndef __RV_LINK_LINK_DETAILS_LINK_CONFIG_H__
#define __RV_LINK_LINK_DETAILS_LINK_CONFIG_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/details/link-config.h
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

/* other project header file includes */
#if defined(LINK_GD32VF103C_START)
#include <rv-link/link/arch/gd32vf103c/gd32vf103c-start/link-config.h>
#elif defined(LINK_LONGAN_NANO)
#include <rv-link/link/arch/gd32vf103c/longan-nano/link-config.h>
#elif defined(LINK_LONGAN_NANO_ALT)
#include <rv-link/link/arch/gd32vf103c/longan-nano-alt/link-config.h>
#else
#error No LINK_X defined
#endif

#endif /* __RV_LINK_LINK_DETAILS_LINK_CONFIG_H__ */
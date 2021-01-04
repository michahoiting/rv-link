#ifndef __RV_LINK_LINK_NVM_CONFIG_H__
#define __RV_LINK_LINK_NVM_CONFIG_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/nvm-config.h
 * \brief Access to non-volatile-memory configuration setting.
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
#include <stddef.h>
#include <stdint.h>

/* TODO: Refactor API to contain configuration items, see also #32. */
int rvl_config_write(const uint32_t* config, size_t size);
int rvl_config_read(uint32_t* config, size_t size);

#endif /* __RV_LINK_LINK_NVM_CONFIG_H__ */

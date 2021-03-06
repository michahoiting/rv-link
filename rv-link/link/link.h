#ifndef __RV_LINK_LINK_LINK_H__
#define __RV_LINK_LINK_LINK_H__
/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/link.h
 * \brief Interface of generic link related functions.
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

const char* rvl_link_get_name(void);
const char* rvl_link_get_unique_dev_id(char *str, size_t len);

#endif /* __RV_LINK_LINK_LINK_H__ */

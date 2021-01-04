#ifndef __RV_LINK_LINK_JTAG_H__
#define __RV_LINK_LINK_JTAG_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/jtag.h
 * \brief JTAG functions
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

/* own component header file includes */
#include <rv-link/link/jtag-inline.h>

#include <stdint.h>

void rvl_jtag_init(void);
void rvl_jtag_fini(void);
void rvl_jtag_srst_put(int srst);

#endif /* __RV_LINK_LINK_JTAG_H__ */

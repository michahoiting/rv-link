#ifndef __RV_LINK_DETAILS_ASSERT_H__
#define __RV_LINK_DETAILS_ASSERT_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/details/assert.h
 * \brief Support for assertion checks.
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
#include <rv-link/link/led.h>
#include <rv-link/link/serial.h>

#ifdef RVL_ASSERT_EN
// TODO Also log using RVL_DEBUG_LOG()
#define rvl_assert(cond) \
  do { \
    if (!(cond)) {  \
      rvl_led_assert(1); \
      for(;;) {} } \
  } while (0)
#else
#define rvl_assert(cond) ((void)0)
#endif

#endif /* __RV_LINK_DETAILS_ASSERT_H__ */

/*
Copyright (c) 2019 zoomdy@163.com
RV-LINK is licensed under the Mulan PSL v1.
You can use this software according to the terms and conditions of the Mulan PSL v1.
You may obtain a copy of Mulan PSL v1 at:
    http://license.coscl.org.cn/MulanPSL
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
PURPOSE.
See the Mulan PSL v1 for more details.
 */
#ifndef __RVL_ASSERT_H__
#define __RVL_ASSERT_H__

#include "rvl-led.h"
#include "rvl-uart-serial.h"

#ifdef RVL_DEBUG_EN

#define RVL_DEBUG_LOG(pt, message) \
do \
{ \
    PT_WAIT_THREAD((pt), (uart_serial2_puts message)); \
} \
while (0)
#else
#define RVL_DEBUG_LOG(pt, message)
#endif


#ifdef RVL_ASSERT_EN
#define rvl_assert(cond) \
  do { \
    if(!(cond)) { rvl_led_assert(1); for(;;) {} } \
  } while (0)

#else
#define rvl_assert(cond)
#endif

#endif /* __RVL_ASSERT_H__ */

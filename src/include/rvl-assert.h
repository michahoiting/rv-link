#ifndef __RVL_ASSERT_H__
#define __RVL_ASSERT_H__

#ifdef RVL_ASSERT_EN

#include "rvl-led.h"

#define rvl_assert(cond) \
  do { \
    if(!(cond)) { rvl_led_assert(1); for(;;) {} } \
  } while (0)

#else

#define rvl_assert(cond)

#endif

#endif /* __RVL_ASSERT_H__ */

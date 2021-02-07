/**
 *     Copyright (c) 2021, Micha Hoiting.
 *
 * \file  rv-link/details/assert.c
 * \brief Implementation of the rvl_assert.
  */

/* own header file include */
#include <rv-link/details/assert.h>

/* own component header file includes */
#include <rv-link/details/debug.h>

/* other project header file includes */
#include <rv-link/link/led.h>

void rvl_assert_handler(const char *cond, const char *func, size_t line)
{
    RVL_DEBUG_LOG(0, ("(%s) assertion failed at function:%s, line number:%d \n", cond, func, line));

    rvl_led_assert(1);

    for(;;) {
      /* halt */
    }
}

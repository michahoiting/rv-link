#include <stdint.h>

#include "rvl-led.h"
#include "pt/timer.h"


uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp)
{
    struct timer timer;

    timer_set(&timer, 100 * 1000 /* 100 ms */);

    for(;;) {
        rvl_led_assert(1);
        while(!timer_expired(&timer));
        timer_reset(&timer);

        rvl_led_assert(0);
        while(!timer_expired(&timer));
        timer_reset(&timer);
    }

    return 0;
}

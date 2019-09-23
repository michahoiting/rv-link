#include "rvl-button.h"
#include "rvl-led.h"
#include "pt.h"


void rvl_button_init(void) __attribute__((weak));
void rvl_button_init(void)
{

}


int rvl_button_pushed(void) __attribute__((weak));
int rvl_button_pushed(void)
{
    return 0;
}


void rvl_led_init(void) __attribute__((weak));
void rvl_led_init(void)
{

}


PT_THREAD(rvl_led_poll(void)) __attribute__((weak));
PT_THREAD(rvl_led_poll(void))
{
    return PT_ENDED;
}


void rvl_led_gdb_connect(int on) __attribute__((weak));
void rvl_led_gdb_connect(int on)
{

}


void rvl_led_link_run(int on) __attribute__((weak));
void rvl_led_link_run(int on)
{

}


void rvl_led_target_run(int on) __attribute__((weak));
void rvl_led_target_run(int on)
{

}


void rvl_led_assert(int on) __attribute__((weak));
void rvl_led_assert(int on)
{

}


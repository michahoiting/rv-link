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
#include "rvl-button.h"
#include "rvl-led.h"
#include "pt/pt.h"


__attribute__((weak)) void rvl_button_init(void)
{

}


__attribute__((weak)) int rvl_button_pushed(void)
{
    return 0;
}


__attribute__((weak)) void rvl_led_init(void)
{

}


__attribute__((weak)) PT_THREAD(rvl_led_poll(void))
{
    return PT_ENDED;
}


__attribute__((weak)) void rvl_led_gdb_connect(int on)
{

}


__attribute__((weak)) void rvl_led_link_run(int on)
{

}


__attribute__((weak)) void rvl_led_target_run(int on)
{

}


__attribute__((weak)) void rvl_led_assert(int on)
{

}


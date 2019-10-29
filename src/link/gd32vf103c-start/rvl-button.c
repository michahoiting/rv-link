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
#include "gd32vf103.h"
#include "rvl-button.h"

#define BUTTON_PORT    GPIOA
#define BUTTON_PIN     GPIO_PIN_0

void rvl_button_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(BUTTON_PORT, GPIO_MODE_IN_FLOATING, 0, BUTTON_PIN);
}

int rvl_button_pushed(void)
{
    if(gpio_input_bit_get(BUTTON_PORT, BUTTON_PIN)) {
        return 1;
    } else {
        return 0;
    }
}

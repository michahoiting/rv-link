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

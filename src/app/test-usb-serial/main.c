#include "pt/pt.h"
#include "rvl-led.h"
#include "rvl-usb-serial.h"

void task_usb_init(void);
PT_THREAD(task_usb_poll(void));

int main(void)
{
    rvl_led_init();
    usb_serial_init();
    task_usb_init();

    for(;;) {
        usb_serial_recv_poll();
        usb_serial_send_poll();
        task_usb_poll();
    }

    return 0;
}

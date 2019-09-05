#include "pt.h"
#include "interface/led.h"
#include "interface/usb-serial.h"

void task_probe_init(void);
PT_THREAD(task_probe_poll(void));

int main(void)
{
    rvl_led_init();
    usb_serial_init();
    task_probe_init();

    for(;;) {
        usb_serial_recv_poll();
        usb_serial_send_poll();
        task_probe_poll();
    }

    return 0;
}



#include "pt.h"

void task_jtag_init(void);
PT_THREAD(task_jtag_poll(void));

void task_usb_init(void);
PT_THREAD(task_usb_poll(void));

int main(void)
{
    task_jtag_init();
    task_usb_init();

    for(;;) {
        task_jtag_poll();
        task_usb_poll();
    }

    return 0;
}

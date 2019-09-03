#include "pt.h"

void task_usb_init(void);
PT_THREAD(task_usb_poll(void));

int main(void)
{
    task_usb_init();

    for(;;) {
        task_usb_poll();
    }

    return 0;
}

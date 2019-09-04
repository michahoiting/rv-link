#include "interface/usb-serial.h"
#include "gdb-serial.h"


int main(void)
{
    usb_serial_init();
    gdb_serial_init();


    for(;;) {
        usb_serial_recv_poll();
        usb_serial_send_poll();
    }
}

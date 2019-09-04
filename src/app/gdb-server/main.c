#include "interface/usb-serial.h"
#include "gdb-serial.h"
#include "gdb-server.h"


int main(void)
{
    usb_serial_init();
    gdb_serial_init();
    gdb_server_init();


    for(;;) {
        usb_serial_recv_poll();
        usb_serial_send_poll();
        gdb_server_poll();
    }
}

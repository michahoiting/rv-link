#include "pt.h"

void usb_serial_init(void);
PT_THREAD(usb_serial_recv_poll(void));
PT_THREAD(usb_serial_send_poll(void));

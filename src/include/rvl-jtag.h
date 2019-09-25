#ifndef __INTERFACE_JTAG_H__
#define __INTERFACE_JTAG_H__

#include <stdint.h>

void rvl_jtag_init(void);
void rvl_jtag_fini(void);
void rvl_jtag_srst_put(int srst);
void rvl_jtag_delay(uint32_t us);


#endif /* __INTERFACE_JTAG_H__ */

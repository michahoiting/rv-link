#ifndef __INTERFACE_JTAG_H__
#define __INTERFACE_JTAG_H__

#include <stdint.h>

void rvl_jtag_init(void);
void rvl_jtag_fini(void);
void rvl_jtag_tms_put(int tms);
void rvl_jtag_tdi_put(int tdi);
void rvl_jtag_tck_put(int tck);
void rvl_jtag_delay_half_period();
int  rvl_jtag_tdo_get();
void rvl_jtag_srst_put(int srst);
void rvl_jtag_delay(uint32_t us);


#endif /* __INTERFACE_JTAG_H__ */

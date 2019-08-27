#ifndef __RVL_JTAG_H__
#define __RVL_JTAG_H__

void rvl_jtag_init(void);
void rvl_jtag_tms_put(int tms);
void rvl_jtag_tdi_put(int tdi);
void rvl_jtag_tck_put(int tck);
void rvl_jtag_delay_half_period();
int rvl_jtag_tdo_get();

#endif // __RVL_JTAG_H__

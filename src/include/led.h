#ifndef __INTERFACE_LED_H__
#define __INTERFACE_LED_H__

void rvl_led_init(void);
void rvl_led_gdb_connect(int on);
void rvl_led_link_run(int on);
void rvl_led_target_run(int on);
void rvl_led_assert(int on);

#endif /* __INTERFACE_LED_H__ */

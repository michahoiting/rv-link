#ifndef __RVL_SERIAL_H__
#define __RVL_SERIAL_H__

#include <stddef.h>

#include "pt/pt.h"

void rvl_serial_init(void);
size_t rvl_serial_putchar(char c);
size_t rvl_serial_puts(const char *s);
size_t rvl_serial_getchar(char *c);
PT_THREAD(rvl_serial_poll(void));

#endif // __RVL_SERIAL_H__

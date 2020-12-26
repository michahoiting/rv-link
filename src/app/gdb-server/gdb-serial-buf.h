/*
Copyright (c) 2019 zoomdy@163.com
RV-LINK is licensed under the Mulan PSL v1.
You can use this software according to the terms and conditions of the Mulan PSL v1.
You may obtain a copy of Mulan PSL v1 at:
    http://license.coscl.org.cn/MulanPSL
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
PURPOSE.
See the Mulan PSL v1 for more details.
 */
#ifndef __GDB_SERIAL_BUF_H__
#define __GDB_SERIAL_BUF_H__

#include <stddef.h>

#include "pt/pt.h"

void gdb_serial_buf_init(void);
size_t gdb_serial_buf_putchar(char c);
size_t gdb_serial_buf_puts(const char *s);
size_t gdb_serial_buf_getchar(char *c);
PT_THREAD(gdb_serial_buf_poll(void));

#endif // __GDB_SERIAL_BUF_H__

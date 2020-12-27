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

#include "gdb-serial-buf.h"

#define RVL_SERIAL_BUFFER_SIZE 256

typedef struct rvl_serial_s
{
    int head;
    int tail;
    char buffer[RVL_SERIAL_BUFFER_SIZE];
} rvl_serial_t;

static rvl_serial_t rvl_serial_i;
#define self rvl_serial_i


void gdb_serial_buf_init(void)
{
    self.head = 0;
    self.tail = 0;
}


PT_THREAD(gdb_serial_buf_poll(void))
{
    return PT_ENDED;
}


size_t gdb_serial_buf_putchar(char c)
{
    int head = self.head + 1;
    if (head >= RVL_SERIAL_BUFFER_SIZE) {
        head = 0;
    }
    if (head == self.tail) {
        return 0;
    }
    self.buffer[self.head] = c;
    self.head = head;

    return 1;
}


size_t gdb_serial_buf_puts(const char *s)
{
    size_t len;
    size_t ret;

    len = 0;
    while (*s) {
        ret = gdb_serial_buf_putchar(*s);
        if (ret == 0) {
            break;
        }
        s++;
        len++;
    }

    return len;
}


size_t gdb_serial_buf_getchar(char *c)
{
    if (self.tail == self.head) {
        return 0;
    }

    *c = self.buffer[self.tail];

    self.tail++;
    if (self.tail >= RVL_SERIAL_BUFFER_SIZE) {
        self.tail = 0;
    }

    return 1;
}
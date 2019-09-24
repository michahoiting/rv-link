#include "rvl-serial.h"


#define RVL_SERIAL_BUFFER_SIZE      256


typedef struct rvl_serial_s
{
    int head;
    int tail;
    char buffer[RVL_SERIAL_BUFFER_SIZE];
}rvl_serial_t;

static rvl_serial_t rvl_serial_i;
#define self rvl_serial_i


void rvl_serial_init(void)
{
    self.head = 0;
    self.tail = 0;
}


PT_THREAD(rvl_serial_poll(void)) __attribute__((weak));
PT_THREAD(rvl_serial_poll(void))
{
    return PT_ENDED;
}


size_t rvl_serial_putchar(char c)
{
    int head = self.head + 1;
    if(head >= RVL_SERIAL_BUFFER_SIZE) {
        head = 0;
    }
    if(head == self.tail) {
        return 0;
    }
    self.buffer[self.head] = c;
    self.head = head;

    return 1;
}


size_t rvl_serial_puts(const char *s)
{
    size_t len;
    size_t ret;

    len = 0;
    while(*s) {
        ret = rvl_serial_putchar(*s);
        if(ret == 0) {
            break;
        }
        s++;
        len++;
    }

    return len;
}


size_t rvl_serial_getchar(char *c)
{
    if(self.tail == self.head) {
        return 0;
    }

    *c = self.buffer[self.tail];

    self.tail++;
    if(self.tail >= RVL_SERIAL_BUFFER_SIZE) {
        self.tail = 0;
    }

    return 1;
}

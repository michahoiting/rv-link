#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rvl-led.h"
#include "rvl-usb-serial.h"
#include "pt/pt.h"


typedef struct task_usb_s
{
    struct pt pt;

    char serial_buffer_rx[1024];
    char serial_buffer_tx[1024];
    bool serial_buffer_rx_own;
    bool serial_buffer_tx_own;
    size_t serial_rx_len;
    size_t serial_tx_len;
}task_usb_t;

static task_usb_t task_usb_i;
#define self task_usb_i


void task_usb_init(void)
{
    PT_INIT(&self.pt);

    self.serial_buffer_rx_own = false;
    self.serial_buffer_tx_own = true;
    self.serial_rx_len = 0;
}


PT_THREAD(task_usb_poll(void))
{
    uint32_t i;
    const char *p;
    char *q;

    PT_BEGIN(&self.pt);

    while (1) {
        PT_WAIT_UNTIL(&self.pt, self.serial_rx_len > 0);
        self.serial_buffer_rx_own = true;

        PT_WAIT_UNTIL(&self.pt, self.serial_buffer_tx_own);

        self.serial_tx_len = 0;
        p = self.serial_buffer_rx;
        q = self.serial_buffer_tx;

        for(i = 0; i < self.serial_rx_len; i++) {
            if(p[i] >= 'A' && p[i] <= 'Z') {
                q[self.serial_tx_len] = p[i] + ('a' - 'A');
            } else if(p[i] >= 'a' && p[i] <= 'z') {
                q[self.serial_tx_len] = p[i] - ('a' - 'A');
            } else if(p[i] == '\r'){
                q[self.serial_tx_len] = '\r';
                self.serial_tx_len++;
                q[self.serial_tx_len] = '\n';
            } else {
                q[self.serial_tx_len] = p[i];
            }
            self.serial_tx_len++;
        }

        self.serial_rx_len = 0;
        self.serial_buffer_rx_own = false;
        self.serial_buffer_tx_own = false;
    }

    PT_END(&self.pt);
}


bool usb_serial_put_recv_data(const uint8_t* buffer, size_t len)
{
    if(self.serial_buffer_rx_own) {
        return false;
    } else {
        if(sizeof(self.serial_buffer_rx) - self.serial_rx_len > len) {
            rvl_led_link_run(1);
            memcpy(&self.serial_buffer_rx[self.serial_rx_len], buffer, len);
            self.serial_rx_len += len;
            return true;
        } else {
            return false;
        }
    }

    return true;
}


const uint8_t* usb_serial_get_send_data(size_t* len)
{
    if(self.serial_buffer_tx_own) {
        return NULL;
    } else {
        *len = self.serial_tx_len;
        return (uint8_t*)self.serial_buffer_tx;
    }
}


void usb_serial_data_sent(void)
{
    self.serial_buffer_tx_own = true;
    rvl_led_link_run(0);
}


#include <stdio.h>
#include <string.h>

#include "pt.h"
#include "interface/usb-serial.h"
#include "interface/led.h"
#include "rvl-tap.h"
#include "v0p11/dtm.h"
#include "v0p11/dmi.h"
#include "rvl-assert.h"


typedef struct task_probe_s
{
    struct pt pt;
    int i;

    uint32_t old_reg[1];
    uint32_t new_reg[1];

    uint32_t idcode[1];
    uint32_t idcode_version;
    uint32_t idcode_part_number;
    uint32_t idcode_manufid;
    uint32_t idcode_lsb;

    uint32_t dtmcs[1];
    uint32_t dtmcs_version;
    uint32_t dtmcs_abits;
    uint32_t dtmcs_dmistat;
    uint32_t dtmcs_idle;

    uint32_t dmstatus;
    uint32_t dmi_result;

    char serial_buffer[1024];
    bool serial_buffer_probe_own;
    size_t serial_data_len;

    bool probe_start;
}task_probe_t;

static task_probe_t task_probe_i;
#define self task_probe_i


#define print(x...) \
    do { \
        PT_WAIT_UNTIL(&self.pt, self.serial_buffer_probe_own); \
        snprintf(self.serial_buffer, sizeof(self.serial_buffer), x); \
        self.serial_data_len = strlen(self.serial_buffer); \
        self.serial_buffer_probe_own = false; \
    }while(0);


void task_probe_init(void)
{
    PT_INIT(&self.pt);

    self.serial_buffer_probe_own = true;
    self.probe_start = false;
}


PT_THREAD(task_probe_poll(void))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_UNTIL(&self.pt, self.probe_start);
    rvl_led_link_run(1);

    rvl_dmi_init();

    // idcode
    for(self.i = 0; self.i < 3; self.i++) {
        PT_WAIT_THREAD(&self.pt, rvl_dtm_idcode(self.idcode));
    }

//    rvl_assert(self.idcode[0] == 0x4e4796b);
    self.idcode_version = self.idcode[0] >> 28;
    self.idcode_part_number = (self.idcode[0] >> 12) & 0xffff;
    self.idcode_manufid = (self.idcode[0] >> 1) & 0x7ff;
    self.idcode_lsb = self.idcode[0] & 0x1;

    print("IDCODE: %08x\r\n", (int)self.idcode[0]);
    print("DTM Version: %d\r\n", (int)self.idcode_version);

    // dtmcs
    for(self.i = 0; self.i < 3; self.i++) {
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs(self.dtmcs));
    }

//    rvl_assert(self.dtmcs[0] == 0x1450);
    self.dtmcs_version = self.dtmcs[0] & 0xf;
    self.dtmcs_abits = (self.dtmcs[0] >> 4) & 0x3f;
    self.dtmcs_dmistat = (self.dtmcs[0] >> 10) & 0x3;
    self.dtmcs_idle = (self.dtmcs[0] >> 12) & 0x7;

    // dmstatus
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(0x11, &self.dmstatus, &self.dmi_result));
//    rvl_assert(self.dmstatus == 0x3c21);

    rvl_led_link_run(0);
    self.probe_start = false;

    PT_END(&self.pt);
}


bool usb_serial_put_recv_data(const uint8_t* buffer, size_t len)
{
    if(strncmp((char*)buffer, "start", 5) == 0) {
        self.probe_start = true;
    }

    return true;
}


const uint8_t* usb_serial_get_send_data(size_t* len)
{
    if(self.serial_buffer_probe_own) {
        return NULL;
    } else {
        *len = self.serial_data_len;
        return (uint8_t*)self.serial_buffer;
    }
}


void usb_serial_data_sent(void)
{
    self.serial_buffer_probe_own = true;
}

#include <stdio.h>
#include <string.h>

#include "pt.h"
#include "interface/usb-serial.h"
#include "interface/led.h"
#include "rvl-tap.h"
#include "dtm.h"
#include "dmi.h"
#include "dm.h"
#include "rvl-assert.h"


typedef struct task_probe_s
{
    struct pt pt;
    bool probe_start;
    int i;

    rvl_dtm_idcode_t idcode;
    rvl_dtm_dtmcs_t dtmcs;
    uint32_t dmi_result;
    riscv_dm_t dm;

    char serial_buffer[1024];
    bool serial_buffer_probe_own;
    size_t serial_data_len;
}task_probe_t;

static task_probe_t task_probe_i;
#define self task_probe_i


#define print(x...) \
    do { \
        PT_WAIT_UNTIL(&self.pt, self.serial_buffer_probe_own); \
        snprintf(self.serial_buffer, sizeof(self.serial_buffer), x); \
        self.serial_data_len = strlen(self.serial_buffer); \
        self.serial_buffer_probe_own = false; \
    }while(0)

#define exit() \
        do { \
            rvl_dmi_fini(); \
            rvl_led_link_run(0); \
            self.probe_start = false; \
            PT_EXIT(&self.pt); \
        }while(0)

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

    /*
     * idcode *****************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dtm_idcode(&self.idcode));

    if(self.idcode.word == 0 || self.idcode.word == 0xffffffff) {
        print("ERROR: bad idcode: 0x%08x\r\n", (int)self.idcode.word);
        exit();
    } else {
        print("idcode: 0x%08x\r\n", (int)self.idcode.word);
        print("idcode.version: %d\r\n", (int)self.idcode.version);
        print("idcode.PartNumber: 0x%x\r\n", (int)self.idcode.part_number);
        print("idcode.ManufId: 0x%x\r\n", (int)self.idcode.manuf_id);
    }

    /*
     * dtmcs ******************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs(&self.dtmcs));
    if(self.dtmcs.word == 0 || self.dtmcs.word == 0xffffffff) {
        print("ERROR: bad dtmcs: 0x%08x\r\n", (int)self.dtmcs.word);
        exit();
    } else {
        print("dtmcs: 0x%08x\r\n", (int)self.dtmcs.word);
        print("dtmcs.version: %d\r\n", (int)self.dtmcs.version);
        if(self.dtmcs.version != RISCV_DEBUG_VERSION) {
            print("ERROR: this prober only support dtmcs.version = %d, but the target's dtmcs.version = %d\r\n",
                    RISCV_DEBUG_VERSION, (int)self.dtmcs.version);
            exit();
        }
        print("dtmcs.abits: %d\r\n", (int)rvl_dtm_get_dtmcs_abits());
        print("dtmcs.idle: %d\r\n", (int)rvl_dtm_get_dtmcs_idle);
    }

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus), &self.dmi_result));
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read_vector(0, (rvl_dmi_reg_t*)(&self.dm), sizeof(self.dm) / sizeof(rvl_dmi_reg_t), &self.dmi_result));
#elif RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P11
#else
#error
#endif

    exit();
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

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
    int tapnum;
    int tap;
    uint8_t irs[8];
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
    self.tapnum = 0;
    self.tap = 0;
}


PT_THREAD(task_probe_poll(void))
{
    uint32_t ir_in = 0xffffffff;
    uint32_t ir_out;
    static int ir_pre, dr_pre, ir_post, dr_post;
    static int i;

    PT_BEGIN(&self.pt);

    PT_WAIT_UNTIL(&self.pt, self.probe_start);

    rvl_led_link_run(1);
    rvl_dmi_init();

    if(self.tap == 0) {
        self.tapnum = 0;
        print("\r\n\r\nfinding TAPs..................................................\r\n");

        rvl_tap_shift_ir(&ir_out, &ir_in, 32);
        for(i = 0; i < 31; i++) {
            switch(ir_out & 0x3) {
            case 0x0:
            case 0x2:
                self.irs[self.tapnum - 1]++;
                break;
            case 0x1:
                self.tapnum++;
                self.irs[self.tapnum - 1] = 1;
                break;
            case 0x3:
                break;
            }
            ir_out >>= 1;
        }

        if(self.tapnum == 0) {
            print("ERROR: can not found any TAP!\r\n");
            exit();
        } else {
            print("found %d TAP(s)\r\n", self.tapnum);
            for(i = 0; i < self.tapnum; i++) {
                print("  TAP%d ir len: %d\r\n", i, self.irs[i]);
            }
        }
    }

    if(self.tap < self.tapnum) {
        ir_pre = 0;
        dr_pre = 0;
        ir_post = 0;
        dr_post = 0;

        for(i = 0; i < self.tap; i++) {
            ir_pre += self.irs[i];
            dr_pre++;
        }

        for(i = self.tap + 1; i < self.tapnum; i++) {
            ir_post += self.irs[i];
            dr_post++;
        }

        print("\r\n\r\nTAP%d config ==================================================\r\n", self.tap);
        print("  ir pre: %d, ir_post: %d\r\n  dr_pre: %d, dr_post: %d\r\n",
                ir_pre, ir_post, dr_pre, dr_post);
        rvl_tap_config(ir_pre, ir_post, dr_pre, dr_post);

        self.tap++;
        if(self.tap == self.tapnum) {
            self.tap = 0;
        }
    }

//    rvl_tap_config(0, 5, 0, 1);


    /*
     * idcode *****************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dtm_idcode(&self.idcode));

    if(self.idcode.word == 0 || self.idcode.word == 0xffffffff) {
        print("ERROR: bad idcode: 0x%08x\r\n", (int)self.idcode.word);
        exit();
    } else {
        print("\r\nidcode: 0x%08x\r\n", (int)self.idcode.word);
        print("idcode.version: %d\r\n", (int)self.idcode.version);
        print("idcode.part_number: 0x%x\r\n", (int)self.idcode.part_number);
        print("idcode.manuf_id: 0x%x\r\n", (int)self.idcode.manuf_id);
    }

    /*
     * dtmcs ******************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs(&self.dtmcs));
    if(self.dtmcs.word == 0 || self.dtmcs.word == 0xffffffff) {
        print("ERROR: bad dtmcs: 0x%08x\r\n", (int)self.dtmcs.word);
        exit();
    } else {
        print("\r\ndtmcs: 0x%08x\r\n", (int)self.dtmcs.word);
        print("dtmcs.version: %d\r\n", (int)self.dtmcs.version);
        if(self.dtmcs.version != RISCV_DEBUG_VERSION) {
            print("ERROR: this prober only support dtmcs.version = %d, but the target's dtmcs.version = %d\r\n",
                    RISCV_DEBUG_VERSION, (int)self.dtmcs.version);
            exit();
        }
        print("dtmcs.abits: %d\r\n", (int)rvl_dtm_get_dtmcs_abits());
        print("dtmcs.idle: %d\r\n", (int)rvl_dtm_get_dtmcs_idle());
    }

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
    /*
     * dmstatus ***************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
    print("\r\ndmstatus: 0x%08x\r\n", (int)self.dm.dmstatus.reg);
    print("dmstatus.version: %d\r\n", (int)self.dm.dmstatus.version);
    if(self.dm.dmstatus.version != RISCV_DEBUG_VERSION + 1) {
        print("ERROR: this prober only support dmstatus.version = %d, but the target's dmstatus.version = %d\r\n",
                RISCV_DEBUG_VERSION + 1, (int)self.dm.dmstatus.version);
        exit();
    }
    print("dmstatus.confstrptrvalid: %d\r\n", (int)self.dm.dmstatus.confstrptrvalid);
    print("dmstatus.hasresethaltreq: %d\r\n", (int)self.dm.dmstatus.hasresethaltreq);
    print("dmstatus.authenticated: %d\r\n", (int)self.dm.dmstatus.authenticated);
    print("dmstatus.impebreak: %d\r\n", (int)self.dm.dmstatus.impebreak);

    /*
     * hartinfo ***************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_HART_INFO, (rvl_dmi_reg_t*)(&self.dm.hartinfo.reg), &self.dmi_result));
    print("\r\nhartinfo: 0x%08x\r\n", (int)self.dm.hartinfo.reg);
    print("hartinfo.dataaddr: %d\r\n", (int)self.dm.hartinfo.dataaddr);
    print("hartinfo.datasize: %d\r\n", (int)self.dm.hartinfo.datasize);
    print("hartinfo.dataaccess: %d, %s\r\n", (int)self.dm.hartinfo.dataaccess,
            self.dm.hartinfo.dataaccess ? "memory map" : "csr map");
    print("hartinfo.nscratch: %d\r\n", (int)self.dm.hartinfo.nscratch);

    /*
     * abstractcs *************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    print("\r\nabstractcs: 0x%08x\r\n", (int)self.dm.abstractcs.reg);
    print("abstractcs.datacount : %d\r\n", (int)self.dm.abstractcs.datacount);
    print("abstractcs.progbufsize : %d\r\n", (int)self.dm.abstractcs.progbufsize);

    /*
     * sbcs *******************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_SB_CS, (rvl_dmi_reg_t*)(&self.dm.sbcs.reg), &self.dmi_result));
        print("\r\nsbcs: 0x%08x\r\n", (int)self.dm.sbcs.reg);
        print("sbcs.sbaccess8 : %d\r\n", (int)self.dm.sbcs.sbaccess8);
        print("sbcs.sbaccess16 : %d\r\n", (int)self.dm.sbcs.sbaccess16);
        print("sbcs.sbaccess32 : %d\r\n", (int)self.dm.sbcs.sbaccess32);
        print("sbcs.sbaccess64 : %d\r\n", (int)self.dm.sbcs.sbaccess64);
        print("sbcs.sbaccess128 : %d\r\n", (int)self.dm.sbcs.sbaccess128);
        print("sbcs.sbasize : %d bits\r\n", (int)self.dm.sbcs.sbasize);
        print("sbcs.sbversion : %d\r\n", (int)self.dm.sbcs.sbversion);

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read_vector(0, (rvl_dmi_reg_t*)(&self.dm), sizeof(self.dm) / sizeof(rvl_dmi_reg_t), &self.dmi_result));
#elif RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P11
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_INFO, (rvl_dmi_reg_t*)(&self.dm.dminfo.reg), &self.dmi_result));
        print("\r\ndminfo: 0x%08x\r\n", (int)self.dm.dminfo.reg);
        print("dminfo.version: %d\r\n", (int)(self.dm.dminfo.loversion_1_0 | (self.dm.dminfo.loversion_3_2 << 2)));
        print("dminfo.authtype: %d\r\n", (int)self.dm.dminfo.authtype);
        print("dminfo.authenticated: %d\r\n", (int)self.dm.dminfo.authenticated);
        print("dminfo.dramsize: %d words\r\n", (int)self.dm.dminfo.dramsize + 1);
        print("dminfo.access8: %d\r\n", (int)self.dm.dminfo.access8);
        print("dminfo.access16: %d\r\n", (int)self.dm.dminfo.access16);
        print("dminfo.access32: %d\r\n", (int)self.dm.dminfo.access32);
        print("dminfo.access64: %d\r\n", (int)self.dm.dminfo.access64);
        print("dminfo.access128: %d\r\n", (int)self.dm.dminfo.access128);
        print("dminfo.serialcount: %d\r\n", (int)self.dm.dminfo.serialcount);
        print("dminfo.abussize: %d\r\n", (int)self.dm.dminfo.abussize);

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

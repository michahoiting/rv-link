#include <stdio.h>
#include <string.h>

#include "rvl-target-config.h"
#include "pt/pt.h"
#include "rvl-led.h"
#include "rvl-button.h"
#include "rvl-usb-serial.h"
#include "rvl-tap.h"
#include "dtm.h"
#include "dmi.h"
#include "dm.h"
#include "opcodes.h"
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
    uint32_t tselect_max;
}task_probe_t;

static task_probe_t task_probe_i;
#define self task_probe_i


#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
static const char * cmd_err_msg[] = {
        "0 (none)",
        "1 (busy)",
        "2 (not supported)",
        "3 (exception)",
        "4 (halt/resume)",
        "5 (bus)",
        "6 (fixme)",
        "7 (other)"
};
#endif


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


static int button_is_pushed(void)
{
    static int prev = 0;
    int retval;

    int pushed = rvl_button_pushed();
    if(prev == 0 && pushed == 1) {
        retval = 1;
    } else {
        retval = 0;
    }
    prev = pushed;

    return retval;
}


void task_probe_init(void)
{
    PT_INIT(&self.pt);

    self.serial_buffer_probe_own = true;
    self.probe_start = false;
    self.tapnum = 0;
    self.tap = 0;

    rvl_button_init();
}


PT_THREAD(task_probe_poll(void))
{
    uint32_t ir_in = 0xffffffff;
    uint32_t ir_out;
    static int ir_pre, dr_pre, ir_post, dr_post;
    static int i;

    PT_BEGIN(&self.pt);

    PT_WAIT_UNTIL(&self.pt, self.probe_start || button_is_pushed());

    rvl_led_link_run(1);
    rvl_dmi_init();

    /*
     * find all TAPs
     */
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
        if(self.dtmcs.version != RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC) {
            print("ERROR: this prober only support dtmcs.version = %d, but the target's dtmcs.version = %d\r\n",
                    RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC, (int)self.dtmcs.version);
            exit();
        }
        print("dtmcs.abits: %d\r\n", (int)rvl_dtm_get_dtmcs_abits());
        print("dtmcs.idle: %d\r\n", (int)rvl_dtm_get_dtmcs_idle());
    }

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
    /*
     * dmstatus ***************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
    print("\r\ndmstatus: 0x%08x\r\n", (int)self.dm.dmstatus.reg);
    print("dmstatus.version: %d\r\n", (int)self.dm.dmstatus.version);
    if(self.dm.dmstatus.version != RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC + 1) {
        print("ERROR: this prober only support dmstatus.version = %d, but the target's dmstatus.version = %d\r\n",
                RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC + 1, (int)self.dm.dmstatus.version);
        exit();
    }
    print("dmstatus.confstrptrvalid: %d\r\n", (int)self.dm.dmstatus.confstrptrvalid);
    print("dmstatus.hasresethaltreq: %d\r\n", (int)self.dm.dmstatus.hasresethaltreq);
    print("dmstatus.authenticated: %d\r\n", (int)self.dm.dmstatus.authenticated);
    print("dmstatus.impebreak: %d\r\n", (int)self.dm.dmstatus.impebreak);

    /*
     * dmcontrol ***************************************************************
     */
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));
    print("\r\ndmcontrol: 0x%08x\r\n", (int)self.dm.dmcontrol.reg);
    print("dmcontrol.dmactive: %d\r\n", (int)self.dm.dmcontrol.dmactive);

    if(!self.dm.dmcontrol.dmactive) {
        self.dm.dmcontrol.reg = 0;
        self.dm.dmcontrol.dmactive = 1;
        print("active dm...\r\n");
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_CONTROL, (rvl_dmi_reg_t*)(&self.dm.dmcontrol.reg), &self.dmi_result));
        print("dmcontrol: 0x%08x\r\n", (int)self.dm.dmcontrol.reg);
        print("dmcontrol.dmactive: %d\r\n", (int)self.dm.dmcontrol.dmactive);
    }

    print("halt hart...\r\n");
    self.dm.dmcontrol.haltreq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
    print("dmstatus: 0x%08x\r\n", (int)self.dm.dmstatus.reg);
    print("dmstatus.anyhalted: %d\r\n", (int)self.dm.dmstatus.anyhalted);
    print("dmstatus.allhalted: %d\r\n", (int)self.dm.dmstatus.allhalted);

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
     * abstractauto ***********************************************************
     */
    self.dm.abstractauto.reg = 0xffffffff;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_AUTO, (rvl_dmi_reg_t)(self.dm.abstractauto.reg), &self.dmi_result));
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_AUTO, (rvl_dmi_reg_t*)(&self.dm.abstractauto.reg), &self.dmi_result));
    print("\r\nabstractauto: 0x%08x\r\n", (int)self.dm.abstractauto.reg);
    print("abstractauto.autoexecdata : 0x%03x\r\n", (int)self.dm.abstractauto.autoexecdata);
    print("abstractauto.autoexecprogbuf : 0x%04x\r\n", (int)self.dm.abstractauto.autoexecprogbuf);
    self.dm.abstractauto.reg = 0;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_AUTO, (rvl_dmi_reg_t)(self.dm.abstractauto.reg), &self.dmi_result));

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

    /*
     * read GPR
     */
    print("\r\nread x1\r\n");
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }

    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.aarpostincrement = 0;
    self.dm.command_access_register.postexec = 0;
    self.dm.command_access_register.write = 0;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = 0x1001; // X1
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        print("x1: 0x%08x\r\n", (int)self.dm.data[0]);
    }

    /*
     * check aarpostincrement
     */
    print("\r\ncheck aarpostincrement\r\n");
    if(self.dm.abstractcs.cmderr) {
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    }

    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.aarpostincrement = 1;
    self.dm.command_access_register.postexec = 0;
    self.dm.command_access_register.write = 0;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = 0x1001; // X1
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));
    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t*)(&self.dm.command_access_register.reg), &self.dmi_result));
    print("\r\ncommand_access_register: 0x%08x\r\n", (int)self.dm.command_access_register.reg);
    print("command_access_register.aarsize : %d\r\n", (int)self.dm.command_access_register.aarsize);
    print("command_access_register.aarpostincrement : %d\r\n", (int)self.dm.command_access_register.aarpostincrement);
    print("command_access_register.postexec : %d\r\n", (int)self.dm.command_access_register.postexec);
    print("command_access_register.write : %d\r\n", (int)self.dm.command_access_register.write);
    print("command_access_register.transfer : %d\r\n", (int)self.dm.command_access_register.transfer);
    print("command_access_register.regno : 0x%04x\r\n", (int)self.dm.command_access_register.regno);
    if(self.dm.command_access_register.aarpostincrement != 1 || self.dm.command_access_register.regno != 0x1002)
    {
        print("aarpostincrement is not supported!\r\n");
    }

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        print("x1: 0x%08x\r\n", (int)self.dm.data[0]);
    }

    /*
     * read CSR
     */
    print("\r\nread misa\r\n");
    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.aarpostincrement = 0;
    self.dm.command_access_register.postexec = 0;
    self.dm.command_access_register.write = 0;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = CSR_MISA;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        print("misa: 0x%08x\r\n", (int)self.dm.data[0]);
    }

    print("\r\nread dpc\r\n");
    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.aarpostincrement = 0;
    self.dm.command_access_register.postexec = 0;
    self.dm.command_access_register.write = 0;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = CSR_DPC;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        print("dpc: 0x%08x\r\n", (int)self.dm.data[0]);
    }

    /*
     * Quick Access
     */
    print("\r\nresume hart...\r\n");
    self.dm.dmcontrol.haltreq = 0;
    self.dm.dmcontrol.resumereq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
    print("dmstatus: 0x%08x\r\n", (int)self.dm.dmstatus.reg);
    print("dmstatus.anyhalted: %d\r\n", (int)self.dm.dmstatus.anyhalted);
    print("dmstatus.allhalted: %d\r\n", (int)self.dm.dmstatus.allhalted);

    self.dm.progbuf[0] = csrr(S0, CSR_DPC);
    self.dm.progbuf[1] = sw(S0, ZERO, 192);
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_PROG_BUF0, (rvl_dmi_reg_t)(self.dm.progbuf[0]), &self.dmi_result));
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_PROG_BUF1, (rvl_dmi_reg_t)(self.dm.progbuf[1]), &self.dmi_result));

    print("\r\nquick access\r\n");
    for(i = 0; i < 6; i++) {
        self.dm.command_quick_access.cmdtype = RISCV_DM_ABSTRACT_CMD_QUICK_ACCESS;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_quick_access.reg), &self.dmi_result));

        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
        if(self.dm.abstractcs.cmderr) {
            print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
            self.dm.abstractcs.reg = 0;
            self.dm.abstractcs.cmderr = 0x7;
            PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
            break;
        } else {
            PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
            print("quick access %d dpc: 0x%08x\r\n", i, (int)self.dm.data[0]);
        }
    }

    /*
     * Access Memory
     */
    print("\r\naccess memory\r\n");
    print("halt hart...\r\n");
    self.dm.dmcontrol.haltreq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
    print("dmstatus: 0x%08x\r\n", (int)self.dm.dmstatus.reg);
    print("dmstatus.anyhalted: %d\r\n", (int)self.dm.dmstatus.anyhalted);
    print("dmstatus.allhalted: %d\r\n", (int)self.dm.dmstatus.allhalted);

    self.dm.data[1] = 0x08000000;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA1, (rvl_dmi_reg_t)(self.dm.data[1]), &self.dmi_result));

    self.dm.command_access_memory.reg = 0;
    self.dm.command_access_memory.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_MEM;
    self.dm.command_access_memory.aamsize = 2;

    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_quick_access.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {
        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        print("access memory 0x%08x: 0x%08x\r\n", (int)self.dm.data[1], (int)self.dm.data[0]);
    }

    /*
     * probe triggers
     */
    print("\r\nprobe triggers\r\n");
    self.tselect_max = 0;

    self.dm.data[0] = 0xffffffff;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA0, (rvl_dmi_reg_t)(self.dm.data[0]), &self.dmi_result));

    self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
    self.dm.command_access_register.aarsize = 2;
    self.dm.command_access_register.aarpostincrement = 0;
    self.dm.command_access_register.postexec = 0;
    self.dm.command_access_register.write = 1;
    self.dm.command_access_register.transfer = 1;
    self.dm.command_access_register.regno = CSR_TSELECT;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t*)(&self.dm.abstractcs.reg), &self.dmi_result));
    if(self.dm.abstractcs.cmderr) {
        print("abstractcs.cmderr: %s\r\n", cmd_err_msg[self.dm.abstractcs.cmderr]);
        self.dm.abstractcs.reg = 0;
        self.dm.abstractcs.cmderr = 0x7;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CS, (rvl_dmi_reg_t)(self.dm.abstractcs.reg), &self.dmi_result));
    } else {
        self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
        self.dm.command_access_register.aarsize = 2;
        self.dm.command_access_register.aarpostincrement = 0;
        self.dm.command_access_register.postexec = 0;
        self.dm.command_access_register.write = 0;
        self.dm.command_access_register.transfer = 1;
        self.dm.command_access_register.regno = CSR_TSELECT;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        self.tselect_max = self.dm.data[0];
        print("tselect_max: %d\r\n", (int)self.tselect_max);
    }

    for(i = 0; i <= self.tselect_max; i++) {
        self.dm.data[0] = i;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_DATA0, (rvl_dmi_reg_t)(self.dm.data[0]), &self.dmi_result));

        self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
        self.dm.command_access_register.aarsize = 2;
        self.dm.command_access_register.aarpostincrement = 0;
        self.dm.command_access_register.postexec = 0;
        self.dm.command_access_register.write = 1;
        self.dm.command_access_register.transfer = 1;
        self.dm.command_access_register.regno = CSR_TSELECT;
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

        self.dm.command_access_register.cmdtype = RISCV_DM_ABSTRACT_CMD_ACCESS_REG;
        self.dm.command_access_register.aarsize = 2;
        self.dm.command_access_register.aarpostincrement = 0;
        self.dm.command_access_register.postexec = 0;
        self.dm.command_access_register.write = 0;
        self.dm.command_access_register.transfer = 1;
        self.dm.command_access_register.regno = 0x7a4; // tinfo
        PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_ABSTRACT_CMD, (rvl_dmi_reg_t)(self.dm.command_access_register.reg), &self.dmi_result));

        PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_DATA0, (rvl_dmi_reg_t*)(&self.dm.data[0]), &self.dmi_result));
        print("tselect: %d, tinfo: 0x%08x\r\n", i, (int)self.dm.data[0]);
    }

    /*
     * last, resume hart!
     */
    print("\r\nresume hart...\r\n");
    self.dm.dmcontrol.haltreq = 0;
    self.dm.dmcontrol.resumereq = 1;
    PT_WAIT_THREAD(&self.pt, rvl_dmi_write(RISCV_DM_CONTROL, (rvl_dmi_reg_t)(self.dm.dmcontrol.reg), &self.dmi_result));

    PT_WAIT_THREAD(&self.pt, rvl_dmi_read(RISCV_DM_STATUS, (rvl_dmi_reg_t*)(&self.dm.dmstatus.reg), &self.dmi_result));
    print("dmstatus: 0x%08x\r\n", (int)self.dm.dmstatus.reg);
    print("dmstatus.anyhalted: %d\r\n", (int)self.dm.dmstatus.anyhalted);
    print("dmstatus.allhalted: %d\r\n", (int)self.dm.dmstatus.allhalted);

#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
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

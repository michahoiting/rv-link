#ifndef __RISCV_DM_H__
#define __RISCV_DM_H__

/*
 * Debug Module (DM)
 */

#include "riscv-debug-spec.h"
#include "dtm.h"

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13


#define RISCV_DM_DATA0          0x04 // Abstract Data 0 (data0)
#define RISCV_DM_DATA1          0x05
#define RISCV_DM_DATA2          0x06
#define RISCV_DM_DATA3          0x07
#define RISCV_DM_DATA4          0x08
#define RISCV_DM_DATA5          0x09
#define RISCV_DM_DATA6          0x0a
#define RISCV_DM_DATA7          0x0b
#define RISCV_DM_DATA8          0x0c
#define RISCV_DM_DATA9          0x0d
#define RISCV_DM_DATA10         0x0e
#define RISCV_DM_DATA11         0x0f // Abstract Data 11 (data11)

#define RISCV_DM_CONTROL        0x10 // Debug Module Control (dmcontrol)
#define RISCV_DM_STATUS         0x11 // Debug Module Status (dmstatus)
#define RISCV_DM_HART_INFO      0x12 // Hart Info (hartinfo)
#define RISCV_DM_HART_SUM1      0x13 // Halt Summary 1 (haltsum1)
#define RISCV_DM_HA_WINDOW_SEL  0x14 // Hart Array Window Select (hawindowsel)
#define RISCV_DM_HA_WINDOW      0x15 // Hart Array Window (hawindow)
#define RISCV_DM_ABSTRACT_CS    0x16 // Abstract Control and Status (abstractcs)
#define RISCV_DM_ABSTRACT_CMD   0x17 // Abstract Command (command)
#define RISCV_DM_ABSTRACT_AUTO  0x18 // Abstract Command Autoexec (abstractauto)
#define RISCV_DM_CONF_STR_PTR0  0x19 // Configuration String Pointer 0 (confstrptr0)
#define RISCV_DM_CONF_STR_PTR1  0x1a
#define RISCV_DM_CONF_STR_PTR2  0x1b
#define RISCV_DM_CONF_STR_PTR3  0x1c // Configuration String Pointer 3 (confstrptr3)
#define RISCV_DM_NEXT           0x1d // Next Debug Module (nextdm)

#define RISCV_DM_PROG_BUF0      0x20 // Program Buffer 0 (progbuf0)
#define RISCV_DM_PROG_BUF1      0x21
#define RISCV_DM_PROG_BUF2      0x22
#define RISCV_DM_PROG_BUF3      0x23
#define RISCV_DM_PROG_BUF4      0x24
#define RISCV_DM_PROG_BUF5      0x25
#define RISCV_DM_PROG_BUF6      0x26
#define RISCV_DM_PROG_BUF7      0x27
#define RISCV_DM_PROG_BUF8      0x28
#define RISCV_DM_PROG_BUF9      0x29
#define RISCV_DM_PROG_BUF10     0x2a
#define RISCV_DM_PROG_BUF11     0x2b
#define RISCV_DM_PROG_BUF12     0x2c
#define RISCV_DM_PROG_BUF13     0x2d
#define RISCV_DM_PROG_BUF14     0x2e
#define RISCV_DM_PROG_BUF15     0x2f // Program Buffer 15 (progbuf15)

#define RISCV_DM_AUTH_DATA      0x30 // Authentication Data (authdata)
#define RISCV_DM_HALT_SUM2      0x34 // Halt Summary 2 (haltsum2)
#define RISCV_DM_HALT_SUM3      0x35 // Halt Summary 3 (haltsum3)
#define RISCV_DM_SB_ADDR3       0x37 // System Bus Address 127:96 (sbaddress3)
#define RISCV_DM_SB_CS          0x38 // System Bus Access Control and Status (sbcs)
#define RISCV_DM_SB_ADDR0       0x39 // System Bus Address 31:0 (sbaddress0)
#define RISCV_DM_SB_ADDR1       0x3a // System Bus Address 63:32 (sbaddress1)
#define RISCV_DM_SB_ADDR2       0x3b // System Bus Address 95:64 (sbaddress2)

#define RISCV_DM_SB_DATA0       0x3c // System Bus Data 31:0 (sbdata0)
#define RISCV_DM_SB_DATA1       0x3d
#define RISCV_DM_SB_DATA2       0x3e
#define RISCV_DM_SB_DATA3       0x3f // System Bus Data 127:96 (sbdata3)

#define RISCV_DM_HAL_SUM0       0x40 // Halt Summary 0 (haltsum0)


// Abstract Command (command)
#define RISCV_DM_ABSTRACT_CMD_ACCESS_REG        0
#define RISCV_DM_ABSTRACT_CMD_QUICK_ACCESS      1
#define RISCV_DM_ABSTRACT_CMD_ACCESS_MEM        2

typedef union riscv_dmcontrol_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int dmactive: 1;
        unsigned int ndmreset: 1;
        unsigned int clrresethaltreq: 1;
        unsigned int setresethaltreq: 1;
        unsigned int reserved4: 2;
        unsigned int hartselhi: 10;
        unsigned int hartsello: 10;
        unsigned int hasel: 1;
        unsigned int reserved27: 1;
        unsigned int ackhavereset: 1;
        unsigned int hartreset: 1;
        unsigned int resumereq: 1;
        unsigned int haltreq: 1;
    };
}riscv_dmcontrol_t;


typedef union riscv_dmstatus_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int version: 4;
        unsigned int confstrptrvalid: 1;
        unsigned int hasresethaltreq: 1;
        unsigned int authbusy: 1;
        unsigned int authenticated: 1;
        unsigned int anyhalted: 1;
        unsigned int allhalted: 1;
        unsigned int anyrunning: 1;
        unsigned int allrunning: 1;
        unsigned int anyunavail: 1;
        unsigned int allunavail: 1;
        unsigned int anynonexistent: 1;
        unsigned int allnonexistent: 1;
        unsigned int anyresumeack: 1;
        unsigned int allresumeack: 1;
        unsigned int anyhavereset: 1;
        unsigned int allhavereset: 1;
        unsigned int reserved20: 2;
        unsigned int impebreak: 1;
        unsigned int reserved23: 9;
    };
}riscv_dmstatus_t;


typedef union riscv_hartinfo_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int dataaddr: 12;
        unsigned int datasize: 4;
        unsigned int dataaccess: 1;
        unsigned int reserved17: 3;
        unsigned int nscratch: 4;
        unsigned int reserved24: 8;
    };
}riscv_hartinfo_t;


typedef union riscv_abstractcs_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int datacount: 4;
        unsigned int reserved4: 4;
        unsigned int cmderr: 3;
        unsigned int reserved11: 1;
        unsigned int busy: 1;
        unsigned int reserved13: 11;
        unsigned int progbufsize: 5;
        unsigned int reserved29: 3;
    };
}riscv_abstractcs_t;


#define RISCV_DM_CMD_REG        0   // Access Register Command
#define RISCV_DM_CMD_QUICK      1   // Quick Access
#define RISCV_DM_CMD_MEM        2   // Access Memory Command


typedef union riscv_command_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int control: 24;
        unsigned int cmdtype: 8;
    };
}riscv_command_t;


typedef union riscv_command_access_register_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int regno: 16;
        unsigned int write: 1;
        unsigned int transfer: 1;
        unsigned int postexec: 1;
        unsigned int aarpostincrement: 1;
        unsigned int aarsize: 3;
        unsigned int reserved23: 1;
        unsigned int cmdtype: 8;
    };
}riscv_command_access_register_t;


typedef union riscv_command_quick_access_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int reserved0: 24;
        unsigned int cmdtype: 8;
    };
}riscv_command_quick_access_t;


typedef union riscv_command_access_memory_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int reserved0: 14;
        unsigned int target_specific: 2;
        unsigned int write: 1;
        unsigned int reserved17: 2;
        unsigned int aampostincrement: 1;
        unsigned int aamsize: 3;
        unsigned int aamvirtual: 1;
        unsigned int cmdtype: 8;
    };
}riscv_command_access_memory_t;


typedef union riscv_abstractauto_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int autoexecdata: 12;
        unsigned int reserved12: 4;
        unsigned int autoexecprogbuf: 16;
    };
}riscv_abstractauto_t;


typedef union riscv_sbcs_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int sbaccess8: 1;
        unsigned int sbaccess16: 1;
        unsigned int sbaccess32: 1;
        unsigned int sbaccess64: 1;
        unsigned int sbaccess128: 1;
        unsigned int sbasize: 7;
        unsigned int sberror: 3;
        unsigned int sbreadondata: 1;
        unsigned int sbautoincrement: 1;
        unsigned int sbaccess: 3;
        unsigned int sbreadonaddr: 1;
        unsigned int sbbusy: 1;
        unsigned int sbbusyerror: 1;
        unsigned int reserved23: 6;
        unsigned int sbversion: 3;
    };
}riscv_sbcs_t;


typedef struct riscv_dm_s
{
    rvl_dmi_reg_t reserved_0x00[4];
    rvl_dmi_reg_t data[12];
    riscv_dmcontrol_t dmcontrol;
    riscv_dmstatus_t dmstatus;
    riscv_hartinfo_t hartinfo;
    rvl_dmi_reg_t haltsum1;
    rvl_dmi_reg_t hawindowsel;
    rvl_dmi_reg_t hawindow;
    riscv_abstractcs_t abstractcs;
    union {
        riscv_command_t command;
        riscv_command_access_register_t command_access_register;
        riscv_command_quick_access_t command_quick_access;
        riscv_command_access_memory_t command_access_memory;
    };
    riscv_abstractauto_t abstractauto;
    rvl_dmi_reg_t confstrptr[4];
    rvl_dmi_reg_t nextdm;
    rvl_dmi_reg_t reserved_0x1f[1];
    rvl_dmi_reg_t progbuf[16];
    rvl_dmi_reg_t authdata;
    rvl_dmi_reg_t reserved_0x31[3];
    rvl_dmi_reg_t haltsum2;
    rvl_dmi_reg_t haltsum3;
    rvl_dmi_reg_t reserved_0x36[1];
    rvl_dmi_reg_t sbaddress3;
    riscv_sbcs_t sbcs;
    rvl_dmi_reg_t sbaddress[3];
    rvl_dmi_reg_t sbdata[4];
    rvl_dmi_reg_t haltsum0;
}riscv_dm_t;

#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
#define RISCV_DM_CONTROL        0x10 // Control (dmcontrol, at 0x10)
#define RISCV_DM_INFO           0x11 // Info (dminfo, at 0x11)
#define RISCV_DM_AUTH_DATA0     0x12 // Authentication Data (authdata0, at 0x12)
#define RISCV_DM_AUTH_DATA1     0x13 // Authentication Data (authdata1, at 0x13)
#define RISCV_DM_SER_DATA       0x14 // Serial Data (serdata, at 0x14)
#define RISCV_DM_SER_STATUS     0x15 // Serial Status (serstatus, at 0x15)
#define RISCV_DM_SB_ADDR0       0x16 // System Bus Address 31:0 (sbaddress0, at 0x16)
#define RISCV_DM_SB_ADDR1       0x17 // System Bus Address 63:32 (sbaddress1, at 0x17)
#define RISCV_DM_SB_DATA0       0x18 // System Bus Data 31:0 (sbdata0, at 0x18)
#define RISCV_DM_SB_DATA1       0x19 // System Bus Data 63:32 (sbdata1, at 0x19)
#define RISCV_DM_HALT_SUM       0x1b // Halt Notification Summary (haltsum, at 0x1b)
#define RISCV_DM_SB_ADDR2       0x3d // System Bus Address 95:64 (sbaddress2, at 0x3d)
#define RISCV_DM_SB_DATA2       0x3e // System Bus Data 95:64 (sbdata2, at 0x3e)
#define RISCV_DM_SB_DATA3       0x3f // System Bus Data 127:96 (sbdata3, at 0x3f)

typedef union riscv_dmcontrol_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int fullreset: 1;
        unsigned int ndreset: 1;
        unsigned int hartid: 10;
        unsigned int access: 3;
        unsigned int autoincrement: 1;
        unsigned int serial: 3;
        unsigned int buserror: 3;
        unsigned int reserved22: 10;
        unsigned int haltnot: 1;
        unsigned int interrupt: 1;
    };
}riscv_dmcontrol_t;

typedef union riscv_dminfo_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int loversion_1_0: 2;
        unsigned int authtype: 2;
        unsigned int authbusy: 1;
        unsigned int authenticated: 1;
        unsigned int loversion_3_2: 2;
        unsigned int reserved8: 1;
        unsigned int haltsum: 1;
        unsigned int dramsize: 6;
        unsigned int access8: 1;
        unsigned int access16: 1;
        unsigned int access32: 1;
        unsigned int access64: 1;
        unsigned int access128: 1;
        unsigned int serialcount: 4;
        unsigned int abussize: 7;
    };
}riscv_dminfo_t;

typedef union riscv_serdata_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int data: 32;
        unsigned int full_overflow: 1;
        unsigned int write_valid: 1;
    };
}riscv_serdata_t;

typedef union riscv_serstatus_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int full_overflow0: 1;
        unsigned int valid0: 1;
        unsigned int full_overflow1: 1;
        unsigned int valid1: 1;
        unsigned int full_overflow2: 1;
        unsigned int valid2: 1;
        unsigned int full_overflow3: 1;
        unsigned int valid3: 1;
        unsigned int full_overflow4: 1;
        unsigned int valid4: 1;
        unsigned int full_overflow5: 1;
        unsigned int valid5: 1;
        unsigned int full_overflow6: 1;
        unsigned int valid6: 1;
        unsigned int full_overflow7: 1;
        unsigned int valid7: 1;
    };
}riscv_serstatus_t;

typedef union riscv_sbaddress_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int data: 32;
        unsigned int read: 1;
        unsigned int busy: 1;
    };
}riscv_sbaddress_t;

typedef union riscv_sbdata_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int data: 32;
        unsigned int write_busy: 1;
        unsigned int read_valid: 1;
    };
}riscv_sbdata_t;

typedef union riscv_haltsum_u
{
    rvl_dmi_reg_t reg;
    struct {
        unsigned int ack: 32;
        unsigned int serialvalid: 1;
        unsigned int serialfull: 1;
    };
}riscv_haltsum_t;

typedef struct riscv_dm_s
{
    rvl_dmi_reg_t dram[16];
    riscv_dmcontrol_t dmcontrol;
    riscv_dminfo_t dminfo;
    rvl_dmi_reg_t authdata[2];
    riscv_serdata_t serdata;
    riscv_serstatus_t serstatus;
    riscv_sbaddress_t sbaddress[2];
    riscv_sbdata_t sbdata[2];
    rvl_dmi_reg_t reserved_0x1a[1];
    riscv_haltsum_t haltsum;
    rvl_dmi_reg_t halt[33];
    riscv_sbaddress_t sbaddress3;
    riscv_sbdata_t sbdata3;
    riscv_sbdata_t sbdata4;
}riscv_dm_t;

#else
#error
#endif


void rvl_dm_init(void);
void rvl_dm_fini(void);

#endif /* __RISCV_DM_H__ */

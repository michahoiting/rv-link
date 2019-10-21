#ifndef __RVL_TARGET_CONFIG_H__
#define __RVL_TARGET_CONFIG_H__

#include "riscv-rv32.h"
#include "riscv-debug-spec-v0p13.h"

/*
 * 3.12.8 Abstract Command Autoexec (abstractauto, at 0x18)
 * This register is optional. Including it allows more efficient burst accesses.
 * A debugger can detect whether it is support by setting bits and reading them back.
 */
#define RVL_TARGET_CONFIG_HAS_ABS_CMD_AUTO

/*
 * aarpostincrement
 * After a successful register access, regno is incremented (wrapping around to 0).
 * Supporting this variant is optional.
 * GD32VF103 可以设置 aarpostincrement，但是 regno 并没有自增
 */
//#define RVL_TARGET_CONFIG_HAS_AAR_POST_INC


#define RVL_TARGET_CONFIG_TAP_IR_POST         5
#define RVL_TARGET_CONFIG_TAP_DR_POST         1

#define RVL_TARGET_CONFIG_HARDWARE_BREAKPOINT_NUM       4

#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR
#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR_UMODE
#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR_DMODE
#define RVL_TARGET_CONFIG_RISCV_FEATURE_VIRTUAL

#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR_CUSTOM \
        "<reg name=\"mtvt\" bitsize=\"32\" regnum=\"840\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"mintstatus\" bitsize=\"32\" regnum=\"903\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"mscratchcsw\" bitsize=\"32\" regnum=\"905\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"mscratchcswl\" bitsize=\"32\" regnum=\"906\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"mnvec\" bitsize=\"32\" regnum=\"2052\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"msubm\" bitsize=\"32\" regnum=\"2053\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"mmisc_ctl\" bitsize=\"32\" regnum=\"2065\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"msavestatus\" bitsize=\"32\" regnum=\"2071\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"msaveepc1\" bitsize=\"32\" regnum=\"2072\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"msavecause1\" bitsize=\"32\" regnum=\"2073\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"msaveepc2\" bitsize=\"32\" regnum=\"2074\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"msavecause2\" bitsize=\"32\" regnum=\"2075\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"mtvt2\" bitsize=\"32\" regnum=\"2093\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"sleepvalue\" bitsize=\"32\" regnum=\"2130\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"txevt\" bitsize=\"32\" regnum=\"2131\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"wfe\" bitsize=\"32\" regnum=\"2129\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#endif /* __RVL_TARGET_CONFIG_H__ */

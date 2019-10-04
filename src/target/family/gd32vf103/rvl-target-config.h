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

#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR
#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR_UMODE
#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR_DMODE
#define RVL_TARGET_CONFIG_RISCV_FEATURE_VIRTUAL

#define RVL_TARGET_CONFIG_RISCV_FEATURE_CSR_CUSTOM \
        "<reg name=\"pushmcause\" bitsize=\"32\" regnum=\"2030\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"pushmepc\" bitsize=\"32\" regnum=\"2031\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"sleepvalue\" bitsize=\"32\" regnum=\"2065\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"txevt\" bitsize=\"32\" regnum=\"2066\" save-restore=\"no\" type=\"int\" group=\"csr\"/>" \
        "<reg name=\"wfe\" bitsize=\"32\" regnum=\"2064\" save-restore=\"no\" type=\"int\" group=\"csr\"/>"

#endif /* __RVL_TARGET_CONFIG_H__ */

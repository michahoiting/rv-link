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

#ifndef __RV_LINK_TARGET_FAMILY_GD32VF103_TARGET_CONFIG_H__
#define __RV_LINK_TARGET_FAMILY_GD32VF103_TARGET_CONFIG_H__

#include <rv-link/target/arch/riscv/riscv-rv32.h>
#include <rv-link/target/arch/riscv/riscv-debug-spec-v0p13.h>

/*
 * 3.12.8 Abstract Command Autoexec (abstractauto, at 0x18)
 * This register is optional. Including it allows more efficient burst accesses.
 * A debugger can detect whether it is support by setting bits and reading them back.
 */
#define RVL_TARGET_CONFIG_HAS_ABS_CMD_AUTO

/*
 * aarpostincrement
 *
 * After a successful register access, regno is incremented (wrapping around to 0).
 * Supporting this variant is optional.
 * GD32VF103 can set aarpostincrement, but regno does not increase automatically
 */
//#define RVL_TARGET_CONFIG_HAS_AAR_POST_INC


#ifndef RVL_TARGET_CONFIG_TAP_DYN
#define RVL_TARGET_CONFIG_TAP_IR_POST         5
#define RVL_TARGET_CONFIG_TAP_DR_POST         1
#endif /* RVL_TARGET_CONFIG_TAP_DYN */

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

#endif /* __RV_LINK_TARGET_FAMILY_GD32VF103_TARGET_CONFIG_H__ */

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
#ifndef __RV_LINK_TARGET_ARCH_RISCV_RISCV_DMI_H__
#define __RV_LINK_TARGET_ARCH_RISCV_RISCV_DMI_H__

#include <stdint.h>

#include <pt/pt.h>

#include <rv-link/target/target-config.h>

/*
 * Debug Module Interface (DMI)
 */

#define RISCV_DMI_OP_NOP        0
#define RISCV_DMI_OP_READ       1
#define RISCV_DMI_OP_WRITE      2

#define RISCV_DMI_RESULT_DONE   0
#define RISCV_DMI_RESULT_FAIL   2
#define RISCV_DMI_RESULT_BUSY   3

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
    typedef uint32_t rvl_dmi_reg_t;
#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
    typedef uint64_t rvl_dmi_reg_t;
#else
#error No RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC defined
#endif

void rvl_dmi_init(void);
void rvl_dmi_fini(void);
PT_THREAD(rvl_dmi_nop(void));
PT_THREAD(rvl_dmi_read(uint32_t addr, rvl_dmi_reg_t* data, uint32_t *result));
PT_THREAD(rvl_dmi_write(uint32_t addr, rvl_dmi_reg_t data, uint32_t *result));
PT_THREAD(rvl_dmi_read_vector(uint32_t start_addr, rvl_dmi_reg_t* buffer, uint32_t len, uint32_t *result));
PT_THREAD(rvl_dmi_write_vector(uint32_t start_addr, const rvl_dmi_reg_t* buffer, uint32_t len, uint32_t *result));

#endif /* __RV_LINK_TARGET_ARCH_RISCV_RISCV_DMI_H__ */

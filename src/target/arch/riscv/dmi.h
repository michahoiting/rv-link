#ifndef __RISCV_DMI_H__
#define __RISCV_DMI_H__

/*
 * Debug Module Interface (DMI)
 */

#include <stdint.h>
#include "riscv-debug-spec.h"
#include "pt/pt.h"
#include "dtm.h"


#define RISCV_DMI_OP_NOP        0
#define RISCV_DMI_OP_READ       1
#define RISCV_DMI_OP_WRITE      2

#define RISCV_DMI_RESULT_DONE   0
#define RISCV_DMI_RESULT_FAIL   2
#define RISCV_DMI_RESULT_BUSY   3


void rvl_dmi_init(void);
void rvl_dmi_fini(void);
PT_THREAD(rvl_dmi_nop(void));
PT_THREAD(rvl_dmi_read(uint32_t addr, rvl_dmi_reg_t* data, uint32_t *result));
PT_THREAD(rvl_dmi_write(uint32_t addr, rvl_dmi_reg_t data, uint32_t *result));
PT_THREAD(rvl_dmi_read_vector(uint32_t start_addr, rvl_dmi_reg_t* buffer, uint32_t len, uint32_t *result));
PT_THREAD(rvl_dmi_write_vector(uint32_t start_addr, const rvl_dmi_reg_t* buffer, uint32_t len, uint32_t *result));

#endif /* __RISCV_DMI_H__ */

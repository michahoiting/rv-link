#ifndef __RISCV_DMI_H__
#define __RISCV_DMI_H__

/*
 * Debug Module Interface (DMI)
 */

#include <stdint.h>
#include "pt.h"

void rvl_dmi_init(void);
PT_THREAD(rvl_dmi_nop(void));
PT_THREAD(rvl_dmi_read(uint32_t addr, uint32_t* data, uint32_t *result));
PT_THREAD(rvl_dmi_write(uint32_t addr, uint32_t data, uint32_t *result));
PT_THREAD(rvl_dmi_read_vector(uint32_t start_addr, uint32_t* buffer, uint32_t len, uint32_t *result));
PT_THREAD(rvl_dmi_write_vector(uint32_t start_addr, const uint32_t* buffer, uint32_t len, uint32_t *result));

#endif /* __RISCV_DMI_H__ */

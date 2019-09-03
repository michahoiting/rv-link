#ifndef __RISCV_DTM_JTAG_H__
#define __RISCV_DTM_JTAG_H__

/*
 * Debug Transport Module (DTM)
 */

#include "common/dtm.h"

PT_THREAD(rvl_dtm_dmi(uint32_t addr, uint32_t* data, uint32_t* op));

#endif /* __RISCV_DTM_JTAG_H__ */

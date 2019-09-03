#ifndef __RISCV_DTM_JTAG_H__
#define __RISCV_DTM_JTAG_H__

/*
 * Debug Transport Module (DTM)
 */

#include <stdint.h>
#include "riscv-debug-spec.h"
#include "pt.h"

void rvl_dtm_init(void);
PT_THREAD(rvl_dtm_idcode(uint32_t* idcode));
PT_THREAD(rvl_dtm_dtmcs(uint32_t* dtmcs));
PT_THREAD(rvl_dtm_dtmcs_dmireset(void));
PT_THREAD(rvl_dtm_dmi(uint32_t addr, uint32_t* data, uint32_t* op));
PT_THREAD(rvl_dtm_run(uint32_t ticks));


#endif /* __RISCV_DTM_JTAG_H__ */

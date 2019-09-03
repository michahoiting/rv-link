#ifndef __RISCV_COMMON_DTM_H__
#define __RISCV_COMMON_DTM_H__

/*
 * Debug Transport Module (DTM)
 */

#include <stdint.h>
#include "riscv-debug-spec.h"
#include "pt.h"


#define RISCV_DTM_JTAG_REG_IDCODE           0x01
#define RISCV_DTM_JTAG_REG_DTMCS            0x10
#define RISCV_DTM_JTAG_REG_DMI              0x11
#define RISCV_DTM_JTAG_REG_BYPASS           0x1f


#define RISCV_DTMCS_DMI_RESET           (1 << 16)
#define RISCV_DTMCS_DMI_HARD_RESET      (1 << 17)


void rvl_dtm_init(void);
PT_THREAD(rvl_dtm_idcode(uint32_t* idcode));
PT_THREAD(rvl_dtm_dtmcs(uint32_t* dtmcs));
PT_THREAD(rvl_dtm_dtmcs_dmireset(void));
PT_THREAD(rvl_dtm_dtmcs_dmihardreset(void));
PT_THREAD(rvl_dtm_run(uint32_t ticks));

#endif /* __RISCV_COMMON_DTM_H__ */

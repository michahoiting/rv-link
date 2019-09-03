#ifndef __RISCV_DTM_JTAG_H__
#define __RISCV_DTM_JTAG_H__

/*
 * Debug Transport Module (DTM)
 */

#include <stdint.h>
#include "riscv-debug.h"
#include "pt.h"

void rvl_dtm_init(void);

PT_THREAD(rvl_dtm_idcode_back_compat(uint32_t* idcode));

PT_THREAD(rvl_dtm_dtmcs_back_compat(uint32_t* dtmcs));
PT_THREAD(rvl_dtm_dtmcs_v0p11(uint32_t* dtmcs));

PT_THREAD(rvl_dtm_dtmcs_dmireset_back_compat(void));
PT_THREAD(rvl_dtm_dtmcs_dmihardreset_v0p13(void));

PT_THREAD(rvl_dtm_dmi_back_compat(uint32_t addr, uint32_t* data, uint32_t* op));
PT_THREAD(rvl_dtm_dmi_v0p11(uint32_t addr, uint32_t* data, uint32_t* op));
PT_THREAD(rvl_dtm_dmi_v0p13(uint32_t addr, uint32_t* data, uint32_t* op));


PT_THREAD(rvl_dtm_run(uint32_t ticks));

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_BACK_COMPAT
    #define rvl_dtm_idcode              rvl_dtm_idcode_back_compat
    #define rvl_dtm_dtmcs               rvl_dtm_dtmcs_back_compat
    #define rvl_dtm_dtmcs_dmireset      rvl_dtm_dtmcs_dmireset_back_compat
    #define rvl_dtm_dmi                 rvl_dtm_dmi_back_compat
#else

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
    #define rvl_dtm_idcode              rvl_dtm_idcode_back_compat
    #define rvl_dtm_dtmcs               rvl_dtm_dtmcs_v0p13
    #define rvl_dtm_dtmcs_dmireset      rvl_dtm_dtmcs_dmireset_back_compat
    #define rvl_dtm_dtmcs_dmihardreset  rvl_dtm_dtmcs_dmihardreset_v0p13
    #define rvl_dtm_dmi                 rvl_dtm_dmi_v0p13
#elif RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P11
    #define rvl_dtm_idcode              rvl_dtm_idcode_back_compat
    #define rvl_dtm_dtmcs               rvl_dtm_dtmcs_v0p11
    #define rvl_dtm_dtmcs_dmireset      rvl_dtm_dtmcs_dmireset_back_compat
    #define rvl_dtm_dmi                 rvl_dtm_dmi_v0p11
#else
    #error "RISCV_DEBUG_VERSION is not supported!"
#endif

#endif

#endif /* __RISCV_DTM_JTAG_H__ */

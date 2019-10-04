#ifndef __RISCV_DTM_JTAG_H__
#define __RISCV_DTM_JTAG_H__

/*
 * Debug Transport Module (DTM)
 */

#include <stdint.h>
#include "riscv-debug-spec.h"
#include "pt/pt.h"


#define RISCV_DTM_JTAG_REG_IDCODE           0x01
#define RISCV_DTM_JTAG_REG_DTMCS            0x10
#define RISCV_DTM_JTAG_REG_DMI              0x11
#define RISCV_DTM_JTAG_REG_BYPASS           0x1f


#define RISCV_DTMCS_DMI_RESET           (1 << 16)
#define RISCV_DTMCS_DMI_HARD_RESET      (1 << 17)


typedef union rvl_dtm_idcode_u {
    uint32_t word;
    struct {
        unsigned int fixed_one:     1;
        unsigned int manuf_id:      11;
        unsigned int part_number:   16;
        unsigned int version:       4;
    };
}rvl_dtm_idcode_t;

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
typedef union rvl_dtm_dtmcs_u
{
    uint32_t word;
    struct {
        unsigned int version:       4;
        unsigned int abits:         6;
        unsigned int dmistat:       2;
        unsigned int idle:          3;
        unsigned int reserved15:    1;
        unsigned int dmireset:      1;
        unsigned int dmihardreset:  1;
        unsigned int reserved18:    14;
    };
}rvl_dtm_dtmcs_t;
#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
typedef union rvl_dtm_dtmcs_s
{
    uint32_t word;
    struct {
        unsigned int version:       4;
        unsigned int loabits:       4;
        unsigned int dmistat:       2;
        unsigned int idle:          3;
        unsigned int hiabits:       2;
        unsigned int reserved15:    1;
        unsigned int dmireset:      1;
        unsigned int reserved17:    15;
    };
}rvl_dtm_dtmcs_t;
#else
#error
#endif


#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
    typedef uint32_t rvl_dmi_reg_t;
#elif RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P11
    typedef uint64_t rvl_dmi_reg_t;
#else
#error
#endif


void rvl_dtm_init(void);
void rvl_dtm_fini(void);
PT_THREAD(rvl_dtm_idcode(rvl_dtm_idcode_t* idcode));
PT_THREAD(rvl_dtm_dtmcs(rvl_dtm_dtmcs_t* dtmcs));
PT_THREAD(rvl_dtm_dtmcs_dmireset(void));

#if RVL_TARGET_CONFIG_RISCV_DEBUG_SPEC == RISCV_DEBUG_SPEC_VERSION_V0P13
PT_THREAD(rvl_dtm_dtmcs_dmihardreset(void));
#endif

PT_THREAD(rvl_dtm_dmi(uint32_t addr, rvl_dmi_reg_t* data, uint32_t* op));
PT_THREAD(rvl_dtm_run(uint32_t ticks));
uint32_t rvl_dtm_get_dtmcs_abits(void);
uint32_t rvl_dtm_get_dtmcs_idle(void);



#endif /* __RISCV_DTM_JTAG_H__ */

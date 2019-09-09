#ifndef __RISCV_DTM_JTAG_H__
#define __RISCV_DTM_JTAG_H__

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


typedef struct rvl_dtm_idcode_s {
    union {
        uint32_t word;
        struct {
            unsigned int fixed_one:     1;
            unsigned int manuf_id:      11;
            unsigned int part_number:   16;
            unsigned int version:       4;
        };
    };
}rvl_dtm_idcode_t;

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
typedef struct rvl_dtm_dtmcs_s
{
    union {
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
    };
}rvl_dtm_dtmcs_t;
#elif RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P11
typedef struct rvl_dtm_dtmcs_s
{
    union {
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
    };
}rvl_dtm_dtmcs_t;
#else
#error
#endif


void rvl_dtm_init(void);
PT_THREAD(rvl_dtm_idcode(rvl_dtm_idcode_t* idcode));
PT_THREAD(rvl_dtm_dtmcs(rvl_dtm_dtmcs_t* dtmcs));
PT_THREAD(rvl_dtm_dtmcs_dmireset(void));

#if RISCV_DEBUG_VERSION == RISCV_DEBUG_VERSION_V0P13
PT_THREAD(rvl_dtm_dtmcs_dmihardreset(void));
#endif

PT_THREAD(rvl_dtm_dmi(uint32_t addr, uint32_t* data, uint32_t* op));
PT_THREAD(rvl_dtm_run(uint32_t ticks));



#endif /* __RISCV_DTM_JTAG_H__ */

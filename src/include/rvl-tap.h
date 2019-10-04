#ifndef __RV_LINK_TAP_H__
#define __RV_LINK_TAP_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "rvl-target-config.h"


void rvl_tap_init(void);
void rvl_tap_fini(void);
void rvl_tap_shift(uint32_t* old, uint32_t *new, size_t len, uint8_t pre, uint8_t post);
void rvl_tap_shift_ir(uint32_t* old_ir, uint32_t* new_ir, size_t ir_len);
void rvl_tap_shift_dr(uint32_t* old_dr, uint32_t* new_dr, size_t dr_len);
void rvl_tap_go_idle(void);
void rvl_tap_run(uint32_t ticks);

#ifdef RVL_TARGET_CONFIG_TAP_DYN
void rvl_tap_config(uint8_t ir_pre, uint8_t ir_post, uint8_t dr_pre, uint8_t dr_post);
#endif // RVL_TAP_CONFIG_DYN

#endif /* __RV_LINK_TAP_H__ */

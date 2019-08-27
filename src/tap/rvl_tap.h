#ifndef __RV_LINK_TAP_H__
#define __RV_LINK_TAP_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum rvl_tap_state_e
{
  RVL_TAP_STATE_TEST_LOGIC_RESET = 0,
  RVL_TAP_STATE_RUN_TEST_IDLE,
  RVL_TAP_STATE_SELECT_DR_SCAN,
  RVL_TAP_STATE_SELECT_IR_SCAN,

  RVL_TAP_STATE_CAPTURE_DR,
  RVL_TAP_STATE_SHIFT_DR,
  RVL_TAP_STATE_EXIT1_DR,
  RVL_TAP_STATE_PAUSE_DR,
  RVL_TAP_STATE_EXIT2_DR,
  RVL_TAP_STATE_UPDATE_DR,

  RVL_TAP_STATE_CAPTURE_IR,
  RVL_TAP_STATE_SHIFT_IR,
  RVL_TAP_STATE_EXIT1_IR,
  RVL_TAP_STATE_PAUSE_IR,
  RVL_TAP_STATE_EXIT2_IR,
  RVL_TAP_STATE_UPDATE_IR,
}rvl_tap_state_t;

int rvl_tap_tick(int tms, int tdi);
rvl_tap_state_t rvl_tap_trace_state(int tms);
void rvl_tap_shift(uint32_t* old, uint32_t *new, size_t len);
void rvl_tap_shift_dr(uint32_t* old_dr, uint32_t* new_dr, size_t dr_len);
void rvl_tap_shift_dr(uint32_t* old_dr, uint32_t* new_dr, size_t dr_len);
void rvl_tap_go_idle(void);

#endif /* __RV_LINK_TAP_H__ */

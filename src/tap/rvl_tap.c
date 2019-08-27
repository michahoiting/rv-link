#include "rvl_tap.h"
#include "rvl_assert.h"

void rvl_tap_init(void)
{
  rvl_jtag_tms_put(1);
  rvl_jtag_tdi_put(1);
  rvl_jtag_tck_put(0);
}

int rvl_tap_tick(int tms, int tdi)
{
  int tdo;

  /*
   *     ___
   * ___|   |
   */

  rvl_jtag_tms_put(tms);
  rvl_jtag_tdi_put(tdi);
  rvl_jtag_delay_half_period();
  rvl_jtag_tck_put(1);
  tdo = rvl_jtag_tdo_get();
  rvl_jtag_delay_half_period();
  rvl_jtag_tck_put(0);

  return tdo;
}


static rvl_tap_state_t _rvl_tap_state = RVL_TAP_STATE_TEST_LOGIC_RESET;
rvl_tap_state_t rvl_tap_state(int tms)
{
  switch(_rvl_tap_state) {
  case RVL_TAP_STATE_TEST_LOGIC_RESET:
    if(!tms) { _rvl_tap_state = RVL_TAP_STATE_RUN_TEST_IDLE;}
    break;

  case RVL_TAP_STATE_RUN_TEST_IDLE:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_SELECT_DR_SCAN;}
    break;

  case RVL_TAP_STATE_SELECT_DR_SCAN:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_SELECT_IR_SCAN;}
    else { _rvl_tap_state = RVL_TAP_STATE_CAPTURE_DR;}
    break;

  case RVL_TAP_STATE_SELECT_IR_SCAN:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_TEST_LOGIC_RESET;}
    else { _rvl_tap_state = RVL_TAP_STATE_CAPTURE_IR;}
    break;

  case RVL_TAP_STATE_CAPTURE_DR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_EXIT1_DR;}
    else { _rvl_tap_state = RVL_TAP_STATE_SHIFT_DR;}
    break;

  case RVL_TAP_STATE_SHIFT_DR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_EXIT1_DR;}
    break;

  case RVL_TAP_STATE_EXIT1_DR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_UPDATE_DR;}
    else { _rvl_tap_state = RVL_TAP_STATE_PAUSE_DR;}
    break;

  case RVL_TAP_STATE_PAUSE_DR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_EXIT2_DR;}
    break;

  case RVL_TAP_STATE_EXIT2_DR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_UPDATE_DR;}
    else { _rvl_tap_state = RVL_TAP_STATE_SHIFT_DR;}
    break;

  case RVL_TAP_STATE_UPDATE_DR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_SELECT_DR_SCAN;}
    else { _rvl_tap_state = RVL_TAP_STATE_RUN_TEST_IDLE;}
    break;

  case RVL_TAP_STATE_CAPTURE_IR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_EXIT1_IR;}
    else { _rvl_tap_state = RVL_TAP_STATE_SHIFT_IR;}
    break;

  case RVL_TAP_STATE_SHIFT_IR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_EXIT1_IR;}
    break;

  case RVL_TAP_STATE_EXIT1_IR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_UPDATE_IR;}
    else { _rvl_tap_state = RVL_TAP_STATE_PAUSE_IR;}
    break;

  case RVL_TAP_STATE_PAUSE_IR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_EXIT2_IR;}
    break;

  case RVL_TAP_STATE_EXIT2_IR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_UPDATE_IR;}
    else { _rvl_tap_state = RVL_TAP_STATE_SHIFT_IR;}
    break;

  case RVL_TAP_STATE_UPDATE_IR:
    if(tms) { _rvl_tap_state = RVL_TAP_STATE_SELECT_DR_SCAN;}
    else { _rvl_tap_state = RVL_TAP_STATE_RUN_TEST_IDLE;}
    break;

  default:
    rvl_assert(0);
    break;
  }

  return _rvl_tap_state;
}


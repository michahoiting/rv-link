#pragma GCC optimize ("O2")

#include "rvl-jtag.h"
#include "rvl-jtag-inline.h"
#include "rvl-tap.h"
#include "rvl-assert.h"


//#define RVL_TARGET_CONFIG_TAP_ASSERT_EN
//#define RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN


#ifdef RVL_TARGET_CONFIG_TAP_ASSERT_EN
#define rvl_tap_assert(x...)    rvl_assert(x)
#else
#define rvl_tap_assert(x...)
#endif


#ifdef RVL_TARGET_CONFIG_TAP_DYN

static uint8_t rvl_tap_ir_pre;
static uint8_t rvl_tap_ir_post;
static uint8_t rvl_tap_dr_pre;
static uint8_t rvl_tap_dr_post;

#define RVL_TARGET_CONFIG_TAP_IR_PRE          rvl_tap_ir_pre
#define RVL_TARGET_CONFIG_TAP_IR_POST         rvl_tap_ir_post
#define RVL_TARGET_CONFIG_TAP_DR_PRE          rvl_tap_dr_pre
#define RVL_TARGET_CONFIG_TAP_DR_POST         rvl_tap_dr_post

#else // RVL_TARGET_CONFIG_TAP_DYN

#ifndef RVL_TARGET_CONFIG_TAP_IR_PRE
#define RVL_TARGET_CONFIG_TAP_IR_PRE          0
#endif

#ifndef RVL_TARGET_CONFIG_TAP_IR_POST
#define RVL_TARGET_CONFIG_TAP_IR_POST         0
#endif

#ifndef RVL_TARGET_CONFIG_TAP_DR_PRE
#define RVL_TARGET_CONFIG_TAP_DR_PRE          0
#endif

#ifndef RVL_TARGET_CONFIG_TAP_DR_POST
#define RVL_TARGET_CONFIG_TAP_DR_POST         0
#endif

#endif // RVL_TARGET_CONFIG_TAP_DYN


#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

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

static rvl_tap_state_t rvl_tap_current_state;

static rvl_tap_state_t rvl_tap_trace_state(int tms);

#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

void rvl_tap_init(void)
{
  rvl_jtag_init();
  rvl_jtag_tms_put(1);
  rvl_jtag_tdi_put(1);
  rvl_jtag_tck_put(0);

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_current_state = RVL_TAP_STATE_TEST_LOGIC_RESET;
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

#ifdef RVL_TARGET_CONFIG_TAP_DYN
  rvl_tap_ir_pre = 0;
  rvl_tap_ir_post = 0;
  rvl_tap_dr_pre = 0;
  rvl_tap_dr_post = 0;
#endif // RVL_TARGET_CONFIG_TAP_DYN
}


void rvl_tap_fini(void)
{
    rvl_jtag_fini();
}


static inline int rvl_tap_tick(int tms, int tdi)
{
  int tdo;

  /*
   *     ___
   * ___|   |
   */

  rvl_jtag_tms_put(tms);
  rvl_jtag_tdi_put(tdi);
  asm volatile("": : :"memory");
  rvl_jtag_tck_low_delay();
  asm volatile("": : :"memory");
  rvl_jtag_tck_put(1);
  asm volatile("": : :"memory");
  tdo = rvl_jtag_tdo_get();
  asm volatile("": : :"memory");
  rvl_jtag_tck_high_delay();
  asm volatile("": : :"memory");
  rvl_jtag_tck_put(0);
  asm volatile("": : :"memory");

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_trace_state(tms);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

  return tdo;
}


#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
static rvl_tap_state_t rvl_tap_trace_state(int tms)
{
  switch(rvl_tap_current_state) {
  case RVL_TAP_STATE_TEST_LOGIC_RESET:
    if(!tms) { rvl_tap_current_state = RVL_TAP_STATE_RUN_TEST_IDLE;}
    break;

  case RVL_TAP_STATE_RUN_TEST_IDLE:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_SELECT_DR_SCAN;}
    break;

  case RVL_TAP_STATE_SELECT_DR_SCAN:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_SELECT_IR_SCAN;}
    else { rvl_tap_current_state = RVL_TAP_STATE_CAPTURE_DR;}
    break;

  case RVL_TAP_STATE_SELECT_IR_SCAN:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_TEST_LOGIC_RESET;}
    else { rvl_tap_current_state = RVL_TAP_STATE_CAPTURE_IR;}
    break;

  case RVL_TAP_STATE_CAPTURE_DR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_EXIT1_DR;}
    else { rvl_tap_current_state = RVL_TAP_STATE_SHIFT_DR;}
    break;

  case RVL_TAP_STATE_SHIFT_DR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_EXIT1_DR;}
    break;

  case RVL_TAP_STATE_EXIT1_DR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_UPDATE_DR;}
    else { rvl_tap_current_state = RVL_TAP_STATE_PAUSE_DR;}
    break;

  case RVL_TAP_STATE_PAUSE_DR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_EXIT2_DR;}
    break;

  case RVL_TAP_STATE_EXIT2_DR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_UPDATE_DR;}
    else { rvl_tap_current_state = RVL_TAP_STATE_SHIFT_DR;}
    break;

  case RVL_TAP_STATE_UPDATE_DR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_SELECT_DR_SCAN;}
    else { rvl_tap_current_state = RVL_TAP_STATE_RUN_TEST_IDLE;}
    break;

  case RVL_TAP_STATE_CAPTURE_IR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_EXIT1_IR;}
    else { rvl_tap_current_state = RVL_TAP_STATE_SHIFT_IR;}
    break;

  case RVL_TAP_STATE_SHIFT_IR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_EXIT1_IR;}
    break;

  case RVL_TAP_STATE_EXIT1_IR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_UPDATE_IR;}
    else { rvl_tap_current_state = RVL_TAP_STATE_PAUSE_IR;}
    break;

  case RVL_TAP_STATE_PAUSE_IR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_EXIT2_IR;}
    break;

  case RVL_TAP_STATE_EXIT2_IR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_UPDATE_IR;}
    else { rvl_tap_current_state = RVL_TAP_STATE_SHIFT_IR;}
    break;

  case RVL_TAP_STATE_UPDATE_IR:
    if(tms) { rvl_tap_current_state = RVL_TAP_STATE_SELECT_DR_SCAN;}
    else { rvl_tap_current_state = RVL_TAP_STATE_RUN_TEST_IDLE;}
    break;

  default:
    rvl_assert(0);
    break;
  }

  return rvl_tap_current_state;
}
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN


void rvl_tap_shift(uint32_t* old, uint32_t *new, size_t len, uint8_t pre, uint8_t post)
{
  // Start state: Select-DR-Scan/Select-IR-Scan
  // End state: Run-Test/Idle

  int tdo;
  int tdi;
  size_t i;

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_SELECT_DR_SCAN
      || rvl_tap_current_state == RVL_TAP_STATE_SELECT_IR_SCAN);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

  rvl_tap_assert(old);
  rvl_tap_assert(new);
  rvl_tap_assert(len > 0);

  rvl_tap_tick(0, 1); // Capture-DR/IR
  rvl_tap_tick(0, 1); // Shift-DR/IR

  for(i = 0; i < pre; i++) {
      rvl_tap_tick(0, 1);
  }

  for(i = 0; i < len; i++) {
    tdi = (new[i / 32] >> (i % 32)) & 1;
    if(i == len - 1 && post == 0) {
      tdo = rvl_tap_tick(1, tdi); // Exit1-DR/IR
    } else {
      tdo = rvl_tap_tick(0, tdi);
    }
    if(tdo) {
        old[i / 32] |= 1 << (i % 32);
    } else {
        old[i / 32] &= ~(1 << (i % 32));
    }
  }

  for(i = 0; i < post; i++) {
      if(i == post - 1) {
          rvl_tap_tick(1, 1); // Exit1-DR/IR
      } else {
          rvl_tap_tick(0, 1);
      }
  }

  rvl_tap_tick(1, 1); // Update-DR/IR
  rvl_tap_tick(0, 1); // Run-Test/Idle

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_RUN_TEST_IDLE);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
}


void rvl_tap_shift_dr(uint32_t* old_dr, uint32_t* new_dr, size_t dr_len)
{
  // Start state: Run-Test/Idle
  // End state: Run-Test/Idle

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_RUN_TEST_IDLE);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

  rvl_tap_tick(1, 1); // Select-DR-Scan

  rvl_tap_shift(old_dr, new_dr, dr_len, RVL_TARGET_CONFIG_TAP_DR_PRE, RVL_TARGET_CONFIG_TAP_DR_POST);
}

void rvl_tap_shift_ir(uint32_t* old_ir, uint32_t* new_ir, size_t ir_len)
{
  // Start state: Run-Test/Idle
  // End state: Run-Test/Idle

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_RUN_TEST_IDLE);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

  rvl_tap_tick(1, 1); // Select-DR-Scan
  rvl_tap_tick(1, 1); // Select-IR-Scan

  rvl_tap_shift(old_ir, new_ir, ir_len, RVL_TARGET_CONFIG_TAP_IR_PRE, RVL_TARGET_CONFIG_TAP_IR_POST);
}

void rvl_tap_go_idle(void)
{
  int i;
  for(i = 0; i < 6; i++){
    rvl_tap_tick(1, 1);
  }
  rvl_tap_tick(0, 1);

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
  rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_RUN_TEST_IDLE);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
}

void rvl_tap_run(uint32_t ticks)
{
    uint32_t i;

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
    rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_RUN_TEST_IDLE);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN

    for(i = 0; i < ticks; i++) {
        rvl_tap_tick(0, 1);
    }

#ifdef RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
    rvl_tap_assert(rvl_tap_current_state == RVL_TAP_STATE_RUN_TEST_IDLE);
#endif // RVL_TARGET_CONFIG_TAP_STATE_TRACE_EN
}


#ifdef RVL_TARGET_CONFIG_TAP_DYN
void rvl_tap_config(uint8_t ir_pre, uint8_t ir_post, uint8_t dr_pre, uint8_t dr_post)
{
    rvl_tap_ir_pre = ir_pre;
    rvl_tap_ir_post = ir_post;
    rvl_tap_dr_pre = dr_pre;
    rvl_tap_dr_post = dr_post;
}
#endif // RVL_TARGET_CONFIG_TAP_DYN

#ifndef __INTERFACE_GDB_CMD_H__
#define __INTERFACE_GDB_CMD_H__


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "pt.h"
#include "rvl-target-def.h"

#ifndef RVL_TARGET_REG_WIDTH
#define RVL_TARGET_REG_WIDTH        32
#endif

#ifndef RVL_TARGET_ADDR_WIDTH
#define RVL_TARGET_ADDR_WIDTH       RVL_TARGET_REG_WIDTH
#endif

#ifndef RVL_TARGET_REG_NUM
#define RVL_TARGET_REG_NUM          33
#endif


#if RVL_TARGET_REG_WIDTH == 32
typedef uint32_t rvl_target_reg_t;
#elif RVL_TARGET_REG_WIDTH == 64
typedef uint64_t rvl_target_ret_t;
#else
#error
#endif

#if RVL_TARGET_ADDR_WIDTH == 32
typedef uint32_t rvl_target_addr_t;
#elif RVL_TARGET_ADDR_WIDTH == 64
typedef uint64_t rvl_target_addr_t;
#else
#error
#endif


void rvl_target_init(void);
PT_THREAD(rvl_target_reset(uint32_t flags));
PT_THREAD(rvl_target_halt(uint32_t* sig));
PT_THREAD(rvl_target_resume(void));
PT_THREAD(rvl_target_step(void));
PT_THREAD(rvl_target_read_registers(rvl_target_reg_t *regs));
PT_THREAD(rvl_target_write_registers(const rvl_target_reg_t *regs));
PT_THREAD(rvl_target_read_memory(uint8_t* mem, rvl_target_addr_t addr, size_t len));
PT_THREAD(rvl_target_write_memory(const uint8_t* mem, rvl_target_addr_t addr, size_t len));


#endif // __INTERFACE_GDB_CMD_H__

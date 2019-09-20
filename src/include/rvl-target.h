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


typedef enum
{
    software_breakpoint = 0,
    hardware_breakpoint = 1,
    write_watchpoint = 2,
    read_watchpoint = 3,
    access_watchpoint = 4,
    unused_breakpoint = 5,
}rvl_target_breakpoint_type_t;


void rvl_target_init(void);
void rvl_target_fini(void);

PT_THREAD(rvl_target_reset(void));
PT_THREAD(rvl_target_halt(void));
PT_THREAD(rvl_target_halt_check(int* halted));
PT_THREAD(rvl_target_resume(void));
PT_THREAD(rvl_target_step(void));

PT_THREAD(rvl_target_read_core_registers(rvl_target_reg_t *regs));
PT_THREAD(rvl_target_write_core_registers(const rvl_target_reg_t *regs));

PT_THREAD(rvl_target_read_register(rvl_target_reg_t *reg, int regno));
PT_THREAD(rvl_target_write_register(rvl_target_reg_t reg, int regno));

PT_THREAD(rvl_target_read_memory(uint8_t* mem, rvl_target_addr_t addr, size_t len));
PT_THREAD(rvl_target_write_memory(const uint8_t* mem, rvl_target_addr_t addr, size_t len));

PT_THREAD(rvl_target_insert_breakpoint(rvl_target_breakpoint_type_t type, rvl_target_addr_t addr, int kind, int* err));
PT_THREAD(rvl_target_remove_breakpoint(rvl_target_breakpoint_type_t type,rvl_target_addr_t addr, int kind, int* err));

const char *rvl_target_get_target_xml(void);
size_t rvl_target_get_target_xml_len(void);

const char *rvl_target_get_memory_map(void);
size_t rvl_target_get_memory_map_len(void);

PT_THREAD(rvl_target_flash_erase(rvl_target_addr_t addr, size_t len, int* err));
PT_THREAD(rvl_target_flash_write(rvl_target_addr_t addr, size_t len, uint8_t* buffer, int* err));
PT_THREAD(rvl_target_flash_done(void));

#endif // __INTERFACE_GDB_CMD_H__

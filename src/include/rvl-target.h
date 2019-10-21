#ifndef __INTERFACE_GDB_CMD_H__
#define __INTERFACE_GDB_CMD_H__


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "pt/pt.h"
#include "rvl-target-config.h"


#if RVL_TARGET_CONFIG_REG_WIDTH == 32
typedef uint32_t rvl_target_reg_t;
#elif RVL_TARGET_CONFIG_REG_WIDTH == 64
typedef uint64_t rvl_target_ret_t;
#else
#error
#endif

#if RVL_TARGET_CONFIG_ADDR_WIDTH == 32
typedef uint32_t rvl_target_addr_t;
#elif RVL_TARGET_CONFIG_ADDR_WIDTH == 64
typedef uint64_t rvl_target_addr_t;
#else
#error
#endif


typedef enum
{
    rvl_target_halt_reason_running = 0,
    rvl_target_halt_reason_hardware_breakpoint = 1,
    rvl_target_halt_reason_software_breakpoint = 2,
    rvl_target_halt_reason_write_watchpoint = 3,
    rvl_target_halt_reason_read_watchpoint = 4,
    rvl_target_halt_reason_access_watchpoint = 5,
    rvl_target_halt_reason_other = 6,
}rvl_target_halt_reason_t;


typedef struct
{
    rvl_target_halt_reason_t reason;
    rvl_target_addr_t addr;
}rvl_target_halt_info_t;

typedef enum
{
    /*
     * 与 gdb Remote Serial Protocol 中 z/Z 命令的 type 字段保持一致
     */
    rvl_target_breakpoint_type_software = 0,
    rvl_target_breakpoint_type_hardware = 1,
    rvl_target_breakpoint_type_write_watchpoint = 2,
    rvl_target_breakpoint_type_read_watchpoint = 3,
    rvl_target_breakpoint_type_access_watchpoint = 4,
    rvl_target_breakpoint_type_unused = 5,
}rvl_target_breakpoint_type_t;


typedef enum
{
    rvl_target_error_none = 0,
    rvl_target_error_line = 1, // 连接线的问题，JTAG 线没有接好，没有供电，逻辑电平不匹配等
    rvl_target_error_compat = 2, // 兼容性问题，
    rvl_target_error_debug_module = 3, // Debug Module 的问题
    rvl_target_error_protected = 4, // target 被保护，必须解除保护才能进行后续的操作
    rvl_target_error_other = 255,
}rvl_target_error_t;


typedef enum {
    rvl_target_memory_type_ram = 0,
    rvl_target_memory_type_flash,
}rvl_target_memory_type_t;

typedef struct {
    rvl_target_memory_type_t type;
    rvl_target_addr_t start;
    rvl_target_addr_t length;
    rvl_target_addr_t blocksize;
} rvl_target_memory_t;


void rvl_target_init(void);
PT_THREAD(rvl_target_init_post(rvl_target_error_t *err));
PT_THREAD(rvl_target_init_after_halted(rvl_target_error_t *err));
PT_THREAD(rvl_target_fini_pre(void));
void rvl_target_fini(void);
void rvl_target_set_error(const char *str);
void rvl_target_get_error(const char **str, uint32_t* pc);
void rvl_target_clr_error(void);

PT_THREAD(rvl_target_reset(void));
PT_THREAD(rvl_target_halt(void));
PT_THREAD(rvl_target_halt_check(rvl_target_halt_info_t* halt_info));
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

const rvl_target_memory_t *rvl_target_get_memory_map(void);
size_t rvl_target_get_memory_map_len(void);

const char *rvl_target_get_name(void);

PT_THREAD(rvl_target_flash_erase(rvl_target_addr_t addr, size_t len, int* err));
PT_THREAD(rvl_target_flash_write(rvl_target_addr_t addr, size_t len, uint8_t* buffer, int* err));
PT_THREAD(rvl_target_flash_done(void));

#endif // __INTERFACE_GDB_CMD_H__

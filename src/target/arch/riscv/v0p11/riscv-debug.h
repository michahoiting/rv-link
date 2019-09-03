#ifndef __RISCV_DEBUG_H__
#define __RISCV_DEBUG_H__

/*
 * 对应 dtmcs 的 version 字段
 */
#define RISCV_DEBUG_VERSION_V0P11           0
#define RISCV_DEBUG_VERSION_V0P13           1
#define RISCV_DEBUG_VERSION_BACK_COMPAT     16


#ifndef RISCV_DEBUG_VERSION
#define RISCV_DEBUG_VERSION         RISCV_DEBUG_VERSION_V0P11
#endif

#endif /* __RISCV_DEBUG_H__ */

#ifndef __RISCV_DEBUG_SPEC_H__
#define __RISCV_DEBUG_SPEC_H__

/*
 * 对应 dtmcs 的 version 字段
 */
#define RISCV_DEBUG_VERSION_V0P11           0
#define RISCV_DEBUG_VERSION_V0P13           1


#ifdef RVL_TARGET_RISCV_DEBUG_VERSION_V0P11
#define RISCV_DEBUG_VERSION         RISCV_DEBUG_VERSION_V0P11
#else
#define RISCV_DEBUG_VERSION         RISCV_DEBUG_VERSION_V0P13
#endif

#endif /* __RISCV_DEBUG_SPEC_H__ */

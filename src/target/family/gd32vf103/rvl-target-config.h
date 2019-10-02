#ifndef __RVL_TARGET_CONFIG_H__
#define __RVL_TARGET_CONFIG_H__

/*
 * 3.12.8 Abstract Command Autoexec (abstractauto, at 0x18)
 * This register is optional. Including it allows more efficient burst accesses.
 * A debugger can detect whether it is support by setting bits and reading them back.
 */
#define RVL_TARGET_CONFIG_HAS_ABS_CMD_AUTO

/*
 * aarpostincrement
 * After a successful register access, regno is incremented (wrapping around to 0).
 * Supporting this variant is optional.
 * GD32VF103 可以设置 aarpostincrement，但是 regno 并没有自增
 */
//#define RVL_TARGET_CONFIG_HAS_AAR_POST_INC

#endif /* __RVL_TARGET_CONFIG_H__ */

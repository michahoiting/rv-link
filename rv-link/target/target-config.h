/*
Copyright (c) 2019 zoomdy@163.com
RV-LINK is licensed under the Mulan PSL v1.
You can use this software according to the terms and conditions of the Mulan PSL v1.
You may obtain a copy of Mulan PSL v1 at:
    http://license.coscl.org.cn/MulanPSL
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
PURPOSE.
See the Mulan PSL v1 for more details.
 */
#ifndef __RV_LINK_TARGET_TARGET_CONFIG_H__
#define __RV_LINK_TARGET_TARGET_CONFIG_H__

#if defined(RVL_TARGET_GD32VF103)
#include <rv-link/target/family/gd32vf103/target-config.h>
#elif defined(RVL_TARGET_RISCV-DEBUG-SPEC-V0P11)
#include <rv-link/target/family/riscv-debug-spec-v0p11/target-config.h>
#elif defined(RVL_TARGET_RISCV-DEBUG-SPEC-V0P13)
#include <rv-link/target/family/riscv-debug-spec-v0p13/target-config.h>
#else
#error No defined RVL_TARGET_X defined
#endif

#endif /* __RV_LINK_TARGET_TARGET_CONFIG_H__ */

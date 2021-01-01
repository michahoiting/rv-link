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
#ifndef __RV_LINK_LINK_JTAG_INLINE_H__
#define __RV_LINK_LINK_JTAG_INLINE_H__

#if defined(LINK_GD32VF103C_START)
#include <rv-link/link/arch/gd32vf103c/gd32vf103c-start/jtag-inline.h>
#elif defined(LINK_LONGAN_NANO)
#include <rv-link/link/arch/gd32vf103c/longan-nano/jtag-inline.h>
#elif defined(LINK_LONGAN_NANO_ALT)
#include <rv-link/link/arch/gd32vf103c/longan-nano-alt/jtag-inline.h>
#endif

#endif /* __RV_LINK_LINK_JTAG_INLINE_H__ */

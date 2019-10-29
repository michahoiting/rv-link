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
#include "pt/clock.h"
#include "riscv_encoding.h"


clock_time_t clock_time(void)
{
    uint32_t mc, mch1, mch2;
    uint64_t cycle;
    uint32_t us;

    do {
        mch1 = read_csr(mcycleh);
        mc = read_csr(mcycle);
        mch2 = read_csr(mcycleh);
    } while(mch1 != mch2);

    cycle = ((uint64_t)mch2 << 32) | mc;
    us = (uint32_t)(cycle / (uint64_t)96);

    return us;
}

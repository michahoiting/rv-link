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

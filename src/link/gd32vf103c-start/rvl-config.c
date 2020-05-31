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
#include "rvl-assert.h"
#include "rvl-link.h"
#include "gd32vf103_fmc.h"

#define RVL_CONFIG_BASE         0x0801FC00
#define RVL_CONFIG_NOT_BASE     (RVL_CONFIG_BASE + 512)


static int rvl_config_write_real(const uint32_t* config, size_t size)
{
    int i;
    const uint32_t* psrc;
    uint32_t addr_dst;
    fmc_state_enum state;

    fmc_unlock();

    state = fmc_page_erase(RVL_CONFIG_BASE);
    if(state != FMC_READY) {
        fmc_lock();
        return -(__LINE__);
    }

    psrc = config;
    addr_dst = RVL_CONFIG_BASE;
    for(i = 0; i < size / 4; i++) {
        state = fmc_word_program(addr_dst, *psrc);
        if(state != FMC_READY) {
            fmc_lock();
            return -(__LINE__);
        }
        addr_dst += 4;
        psrc++;
    }

    psrc = config;
    addr_dst = RVL_CONFIG_NOT_BASE;
    for(i = 0; i < size / 4; i++) {
        state = fmc_word_program(addr_dst, ~*psrc);
        if(state != FMC_READY) {
            fmc_lock();
            return -(__LINE__);
        }
        addr_dst += 4;
        psrc++;
    }

    fmc_lock();
    return (int)size;
}


int rvl_config_write(const uint32_t* config, size_t size)
{
    int i;
    const uint32_t* psrc;
    const uint32_t* pdst;

    rvl_assert(config != NULL);
    rvl_assert(size <= 512);
    rvl_assert(size % 4 == 0);

    psrc = config;
    pdst = (uint32_t*)RVL_CONFIG_BASE;
    for(i = 0; i < size / 4; i++) {
        if(*pdst != *psrc) {
            return rvl_config_write_real(config, size);
        }
        pdst++;
        psrc++;
    }

    return (int)size;
}


int rvl_config_read(uint32_t* config, size_t size)
{
    int i;
    uint32_t* pdst;
    uint32_t* psrc;

    rvl_assert(config != NULL);
    rvl_assert(size <= 512);
    rvl_assert(size % 4 == 0);

    pdst = config;
    psrc = (uint32_t*)RVL_CONFIG_BASE;

    for(i = 0; i < size / 4; i++) {
        *pdst = *psrc;
        psrc++;
        pdst++;
    }

    psrc = (uint32_t*)RVL_CONFIG_NOT_BASE;
    pdst = (uint32_t*)RVL_CONFIG_BASE;
    for(i = 0; i < size / 4; i++) {
        if(*psrc != (~*pdst)) {
            return -(__LINE__);
        }
    }

    return (int)size;
}


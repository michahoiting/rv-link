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
    const uint32_t* p;
    uint32_t addr;
    fmc_state_enum state;

    fmc_unlock();

    state = fmc_page_erase(RVL_CONFIG_BASE);
    if(state != FMC_READY) {
        fmc_lock();
        return -(__LINE__);
    }

    p = config;
    addr = RVL_CONFIG_BASE;
    for(i = 0; i < size / 4; i++) {
        state = fmc_word_program(addr, *p);
        if(state != FMC_READY) {
            fmc_lock();
            return -(__LINE__);
        }
        addr += 4;
        p++;
    }

    p = config;
    addr = RVL_CONFIG_NOT_BASE;
    for(i = 0; i < size / 4; i++) {
        state = fmc_word_program(addr, ~*p);
        if(state != FMC_READY) {
            fmc_lock();
            return -(__LINE__);
        }
        addr += 4;
        p++;
    }

    fmc_lock();
    return (int)size;
}


int rvl_config_write(const uint32_t* config, size_t size)
{
    int i;
    const uint32_t* p;
    const uint32_t* q;

    rvl_assert(config != NULL);
    rvl_assert(size <= 512);
    rvl_assert(size % 4 == 0);

    p = config;
    q = (uint32_t*)RVL_CONFIG_BASE;
    for(i = 0; i < size / 4; i++) {
        if(*q != *p) {
            return rvl_config_write_real(config, size);
        }
        q++;
        p++;
    }

    return (int)size;
}


int rvl_config_read(uint32_t* config, size_t size)
{
    int i;
    uint32_t* p;
    uint32_t* q;

    rvl_assert(config != NULL);
    rvl_assert(size <= 512);
    rvl_assert(size % 4 == 0);

    p = config;
    q = (uint32_t*)RVL_CONFIG_BASE;

    for(i = 0; i < size / 4; i++) {
        *q = *p;
        q++;
        p++;
    }

    q = (uint32_t*)RVL_CONFIG_NOT_BASE;
    p = (uint32_t*)RVL_CONFIG_BASE;
    for(i = 0; i < size / 4; i++) {
        if(*q != (~*p)) {
            return -(__LINE__);
        }
    }

    return (int)size;
}


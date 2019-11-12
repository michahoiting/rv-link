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
#include "gd32vf103.h"
#include "riscv_encoding.h"
#include "rvl-jtag.h"
#include "rvl-jtag-inline.h"
#include "rvl-link-config.h"


void rvl_jtag_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    AFIO_PCF0 = (AFIO_PCF0 & 0xF8FFFFFF) | 0x04000000;

    gpio_init(TCK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, TCK_PIN);
    gpio_init(TMS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, TMS_PIN);
    gpio_init(TDI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, TDI_PIN);
    gpio_init(TDO_PORT, GPIO_MODE_IPU, 0, TDO_PIN);
}


void rvl_jtag_fini(void)
{
    gpio_init(TCK_PORT, GPIO_MODE_AIN, 0, TCK_PIN);
    gpio_init(TMS_PORT, GPIO_MODE_AIN, 0, TMS_PIN);
    gpio_init(TDI_PORT, GPIO_MODE_AIN, 0, TDI_PIN);
    gpio_init(TDO_PORT, GPIO_MODE_AIN, 0, TDO_PIN);
}


void rvl_jtag_srst_put(int srst)
{

}


void rvl_jtag_delay(uint32_t us)
{
    uint32_t start = read_csr(mcycle);
    uint32_t period = us * 96; // CPU run @96MHz

    while(read_csr(mcycle) - start < period);
}

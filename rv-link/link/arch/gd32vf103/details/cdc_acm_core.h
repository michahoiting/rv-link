#ifndef __RV_LINK_LINK_ARCH_G32VF103C_DETAILS_CDC_ACM_CORE_H__
#define __RV_LINK_LINK_ARCH_G32VF103C_DETAILS_CDC_ACM_CORE_H__
/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *     Copyright (c) 2021, Micha Hoiting <micha.hoiting@gmail.com>
 *
 *     \file  rv-link/link/arch/gd32vf103/details/cdc_acm_core.h
 *     \brief The header file of IAP driver.
 *     \version 2019-6-5, V1.0.0, demo for GD32VF103
 *
 *     Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this
 *        list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright notice,
 *        this list of conditions and the following disclaimer in the documentation
 *        and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of its contributors
 *        may be used to endorse or promote products derived from this software without
 *        specific prior written permission.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

/* other library header file includes */
#include <rv-link/link/arch/gd32vf103/details/usbd_conf.h>
#include "drv_usb_dev.h"
#include "nuclei_sdk_soc.h"

extern __IO usb_class_core cdc_acm_usb_class_core;

extern __IO uint8_t cdc_acm_ep0_packet_received;
extern __IO uint8_t cdc_acm_ep0_packet_sent;
extern __IO uint32_t cdc_acm_ep0_packet_length;

extern __IO uint8_t cdc_acm_ep1_packet_received;
extern __IO uint8_t cdc_acm_ep1_packet_sent;
extern __IO uint32_t cdc_acm_ep1_packet_length;

extern __IO uint8_t cdc_acm_ep2_packet_received;
extern __IO uint8_t cdc_acm_ep2_packet_sent;
extern __IO uint32_t cdc_acm_ep2_packet_length;

/* function declarations */
void cdc_acm_init_desc(usb_desc *desc);

#endif  /* __RV_LINK_LINK_ARCH_G32VF103C_DETAILS_CDC_ACM_CORE_H__ */

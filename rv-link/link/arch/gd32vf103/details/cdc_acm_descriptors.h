#ifndef __RV_LINK_LINK_ARCH_G32VF103C_DETAILS_CDC_ACM_DESCRIPTORS_H__
#define __RV_LINK_LINK_ARCH_G32VF103C_DETAILS_CDC_ACM_DESCRIPTORS_H__
/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *     Copyright (c) 2021, Micha Hoiting <micha.hoiting@gmail.com>
 *
 *     \file  rv-link/link/arch/gd32vf103/details/cdc_acm_descriptors.h
 *     \brief The header file of CDC ACM descriptors.
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

/* system library header file includes */
#include <stdint.h>
#include <wchar.h>

/* other library header file includes */
#include "usb_ch9_std.h"

#define USB_DESCTYPE_CS_INTERFACE               0x24
#define USB_CDC_ACM_CONFIG_DESC_SIZE            0x43

#define CDC_ACM_DESC_SIZE                       0x3A

#define CDC_ACM_DESC_TYPE                       0x21

#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define NO_CMD                                  0xFF

#pragma pack(1)

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: header function descriptor */
    uint16_t  bcdCDC;                     /*!< bcdCDC: low byte of spec release number (CDC1.10) */
} usb_descriptor_header_function_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype:  call management function descriptor */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D0 is reset, D1 is ignored */
    uint8_t  bDataInterface;              /*!< bDataInterface: 1 interface used for call management */
} usb_descriptor_call_managment_function_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: abstract control management desc */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D1 */
} usb_descriptor_acm_function_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: union func desc */
    uint8_t  bMasterInterface;            /*!< bMasterInterface: communication class interface */
    uint8_t  bSlaveInterface0;            /*!< bSlaveInterface0: data class interface */
} usb_descriptor_union_function_struct;

#pragma pack()

typedef struct
{
    usb_desc_itf                                      itf_command;
    usb_descriptor_header_function_struct             cdc_header;
    usb_descriptor_call_managment_function_struct     cdc_call_managment;
    usb_descriptor_acm_function_struct                cdc_acm;
    usb_descriptor_union_function_struct              cdc_union;
    usb_desc_ep                                       ep_cmd_in;
    usb_desc_itf                                      itf_data;
    usb_desc_ep                                       ep_data_out;
    usb_desc_ep                                       ep_data_in;
}usb_descriptor_cdc_set_struct_vcom_disable;

typedef struct
{
    usb_desc_config                                   config;
    usb_descriptor_cdc_set_struct_vcom_disable        cdc[1];
} usb_descriptor_configuration_set_struct_vcom_disable;

typedef struct
{
    usb_desc_itf_association                          itf_association;
    usb_desc_itf                                      itf_command;
    usb_descriptor_header_function_struct             cdc_header;
    usb_descriptor_call_managment_function_struct     cdc_call_managment;
    usb_descriptor_acm_function_struct                cdc_acm;
    usb_descriptor_union_function_struct              cdc_union;
    usb_desc_ep                                       ep_cmd_in;
    usb_desc_itf                                      itf_data;
    usb_desc_ep                                       ep_data_out;
    usb_desc_ep                                       ep_data_in;
} usb_descriptor_cdc_set_struct_vcom_enable;

typedef struct {
    usb_desc_header header;
    wchar_t wString[16];
} usb_descriptor_string;

typedef struct
{
    usb_desc_config                                   config;
    usb_descriptor_cdc_set_struct_vcom_enable         cdc[2];
} usb_descriptor_configuration_set_struct_vcom_enable;

extern __I const usb_desc_dev cdc_acm_device_descriptor_vcom_enable;
extern __I const usb_desc_dev cdc_acm_device_descriptor_vcom_disable;
extern __I const usb_descriptor_configuration_set_struct_vcom_disable cdc_acm_configuration_descriptor_vcom_disable;
extern __I const usb_descriptor_configuration_set_struct_vcom_enable cdc_acm_configuration_descriptor_vcom_enable;

/* function declarations */
const void *cdc_acm_get_dev_strings_desc(void);

#endif  /* __RV_LINK_LINK_ARCH_G32VF103C_DETAILS_CDC_ACM_DESCRIPTORS_H__ */

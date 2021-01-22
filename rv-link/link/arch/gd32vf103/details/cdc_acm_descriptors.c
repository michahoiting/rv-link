/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *     Copyright (c) 2021, Micha Hoiting <micha.hoiting@gmail.com>
 *
 *     \file  rv-link/link/arch/gd32vf103/details/cdc_acm_descriptors.c
 *     \brief The implementation file of CDC ACM descriptors.
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

/* own header file include */
#include <rv-link/link/arch/gd32vf103/details/cdc_acm_descriptors.h>

/* system library header file includes */
#include <stddef.h>
#include <stdio.h>

/* other library header file includes */
#include "usbd_enum.h"

/* own component header file includes */
#include <rv-link/link/details/link-config.h>
#include <rv-link/link/link.h>

#define USBD_VID                          0x28e9
#define USBD_PID                          0x018a

extern int rvl_vcom_enable();
static size_t stowcs(wchar_t *dest, const char *str, size_t max);

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
__I usb_desc_dev cdc_acm_device_descriptor =
{
    .header =
     {
         .bLength = USB_DEV_DESC_LEN,
         .bDescriptorType = USB_DESCTYPE_DEV
     },
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xef,                   /* Miscellaneous Device */
    .bDeviceSubClass = 0x02,                /* Common Class */
    .bDeviceProtocol = 0x01,                /* Interface Association */
    .bMaxPacketSize0 = USB_FS_EP0_MAX_LEN,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = STR_IDX_MFC,
    .iProduct = STR_IDX_PRODUCT,
    .iSerialNumber = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

/* USB device configuration descriptor */
__I usb_descriptor_configuration_set_struct_vcom_disable cdc_acm_configuration_descriptor_vcom_disable =
{
    .config =
    {
        .header =
         {
            .bLength = USB_CFG_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength = sizeof(usb_descriptor_configuration_set_struct_vcom_disable),
        .bNumInterfaces = 0x02,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x00,
        .bmAttributes = 0x80,
        .bMaxPower = 0x32
    },

    .cdc[0].itf_command =
    {
        .header =
         {
             .bLength = USB_ITF_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x01,
        .bInterfaceClass = 0x02,
        .bInterfaceSubClass = 0x02,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x04
    },

    .cdc[0].cdc_header =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_header_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x00,
        .bcdCDC = 0x0110
    },

    .cdc[0].cdc_call_managment =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_call_managment_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x01,
        .bmCapabilities = 0x00,
        .bDataInterface = 0x01
    },

    .cdc[0].cdc_acm =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_acm_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x02,
        .bmCapabilities = 0x02,
    },

    .cdc[0].cdc_union =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_union_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x06,
        .bMasterInterface = 0x00,
        .bSlaveInterface0 = 0x01,
    },

    .cdc[0].ep_cmd_in =
    {
        .header =
         {
            .bLength = USB_EP_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_CMD_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval = 0xff
    },

    .cdc[0].itf_data =
    {
        .header =
         {
            .bLength = USB_ITF_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x01,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x02,
        .bInterfaceClass = 0x0A,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x00
    },

    .cdc[0].ep_data_out =
    {
        .header =
         {
             .bLength = USB_EP_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_DATA_OUT_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },

    .cdc[0].ep_data_in =
    {
        .header =
         {
             .bLength = USB_EP_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_DATA_IN_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },
};

/* USB device configuration descriptor */
__I usb_descriptor_configuration_set_struct_vcom_enable cdc_acm_configuration_descriptor_vcom_enable =
{
    .config =
    {
        .header =
         {
            .bLength = USB_CFG_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength = sizeof(usb_descriptor_configuration_set_struct_vcom_enable),
        .bNumInterfaces = 0x04,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x00,
        .bmAttributes = 0x80,
        .bMaxPower = 0x32
    },

    .cdc[0].itf_association = {
        .header =
         {
             .bLength = USB_ITF_ASSOCIATION_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_ITF_ASSOCIATION
         },
         .bFirstInterface = 0x00,
         .bInterfaceCount = 0x02,
         .bFunctoinClass = 0x02,
         .bFunctionSubClass = 0x02,
         .bFunctionProtocol = 0x00,
         .iFunction = 0x00
    },

    .cdc[0].itf_command =
    {
        .header =
         {
             .bLength = USB_ITF_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x01,
        .bInterfaceClass = 0x02,
        .bInterfaceSubClass = 0x02,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x04
    },

    .cdc[0].cdc_header =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_header_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x00,
        .bcdCDC = 0x0110
    },

    .cdc[0].cdc_call_managment =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_call_managment_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x01,
        .bmCapabilities = 0x00,
        .bDataInterface = 0x01
    },

    .cdc[0].cdc_acm =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_acm_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x02,
        .bmCapabilities = 0x02,
    },

    .cdc[0].cdc_union =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_union_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x06,
        .bMasterInterface = 0x00,
        .bSlaveInterface0 = 0x01,
    },

    .cdc[0].ep_cmd_in =
    {
        .header =
         {
            .bLength = USB_EP_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_CMD_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval = 0xff
    },

    .cdc[0].itf_data =
    {
        .header =
         {
            .bLength = USB_ITF_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x01,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x02,
        .bInterfaceClass = 0x0A,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x00
    },

    .cdc[0].ep_data_out =
    {
        .header =
         {
             .bLength = USB_EP_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_DATA_OUT_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },

    .cdc[0].ep_data_in =
    {
        .header =
         {
             .bLength = USB_EP_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_DATA_IN_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },

    .cdc[1].itf_association = {
        .header =
         {
             .bLength = USB_ITF_ASSOCIATION_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_ITF_ASSOCIATION
         },
         .bFirstInterface = 0x02,
         .bInterfaceCount = 0x02,
         .bFunctoinClass = 0x02,
         .bFunctionSubClass = 0x02,
         .bFunctionProtocol = 0x00,
         .iFunction = 0x00
    },

    .cdc[1].itf_command =
    {
        .header =
         {
             .bLength = USB_ITF_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x02,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x01,
        .bInterfaceClass = 0x02,
        .bInterfaceSubClass = 0x02,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x05
    },

    .cdc[1].cdc_header =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_header_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x00,
        .bcdCDC = 0x0110
    },

    .cdc[1].cdc_call_managment =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_call_managment_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x01,
        .bmCapabilities = 0x00,
        .bDataInterface = 0x03
    },

    .cdc[1].cdc_acm =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_acm_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x02,
        .bmCapabilities = 0x02,
    },

    .cdc[1].cdc_union =
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_union_function_struct),
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x06,
        .bMasterInterface = 0x02,
        .bSlaveInterface0 = 0x03,
    },

    .cdc[1].ep_cmd_in =
    {
        .header =
         {
            .bLength = USB_EP_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM1_CMD_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval = 0xff
    },

    .cdc[1].itf_data =
    {
        .header =
         {
            .bLength = USB_ITF_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x03,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x02,
        .bInterfaceClass = 0x0A,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x00
    },

    .cdc[1].ep_data_out =
    {
        .header =
         {
             .bLength = USB_EP_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM1_DATA_OUT_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },

    .cdc[1].ep_data_in =
    {
        .header =
         {
             .bLength = USB_EP_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM1_DATA_IN_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },
};

/* USB language ID Descriptor */
const usb_desc_LANGID usbd_language_id_desc =
{
    .header =
     {
         .bLength = sizeof(usb_desc_LANGID),
         .bDescriptorType = USB_DESCTYPE_STR
     },
    .wLANGID = ENG_LANGID
};

static usb_descriptor_string usb_string_desc_serial =
{
    .header =
    {
         .bLength = 0,
         .bDescriptorType = USB_DESCTYPE_STR
    },
    .wString = {0}
};

const void *usbd_strings[STR_IDX_MAX] = {
    [0x00] = (uint8_t*) &usbd_language_id_desc,
    [0x01] = USBD_STRING_DESC("RV-LINK"),
    [0x02] = USBD_STRING_DESC(RVL_LINK_CONFIG_NAME),
    [0x03] = (uint8_t*) &usb_string_desc_serial,
    [0x04] = USBD_STRING_DESC("RV-Link GDB Server"),
    [0x05] = USBD_STRING_DESC("RV-Link COM Port"),
};

/*!
    \brief      Convert a C string to wide-character string
    \param[in]  dest: pointer to an array of wchar_t elements long enough to
                      contain the resulting sequence (at most, max wide characters)
    \param[in]  str: C string (without multibyte characters to be interpreted)
    \param[in]  max: maximum number of wchar_t characters to write to dest
    \param[out] none
    \retval     The number of wide characters written to dest, not including the
                eventual terminating null character.
*/
static size_t stowcs(wchar_t *dest, const char *str, size_t max)
{
    int i = 0;
    while ((*dest++ = *str++) && max--) i++;
    return i;
}

/*!
    \brief      fill an UBS serial string descriptor
    \param[in]  desc: pointer to USB string descriptor
    \param[in]  str: C string to write to the string descriptor
    \param[out] none
    \retval     none
*/
static void cdc_acm_set_string(usb_descriptor_string *desc, const char *str)
{
    int n = stowcs(desc->wString, str, sizeof(desc->wString));
    desc->header.bLength = sizeof(wchar_t) * n + 2;
}

/*!
    \brief      Get the USB string descriptor table of the CDC ACM device
    \param[in]  none
    \param[out] none
    \retval     none
*/
const void *cdc_acm_get_dev_strings_desc(void)
{
    char tmp[32];

    cdc_acm_set_string((usb_descriptor_string*) usbd_strings[STR_IDX_SERIAL], rvl_link_get_unique_dev_id(tmp, sizeof(tmp)));

    if (!rvl_vcom_enable()) {
        usbd_strings[STR_IDX_ITF] = NULL;
    }

    return usbd_strings;
}

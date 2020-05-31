/*!
    \file  cdc_acm_core.c
    \brief CDC ACM driver

    \version 2019-6-5, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "cdc_acm_core.h"
#include <rvl-link-config.h>

#define USBD_VID                          0x28e9
#define USBD_PID                          0x018a

static uint32_t cdc_cmd = 0xFFU;

uint8_t usb_data_buffer[CDC_ACM_DATA_PACKET_SIZE];
uint8_t usb_cmd_buffer[CDC_ACM_CMD_PACKET_SIZE];

__IO uint8_t packet_sent = 1U;
__IO uint8_t packet_receive = 1U;
__IO uint32_t  receive_length = 0U;

__IO uint8_t packet_sent1 = 1U;
__IO uint8_t packet_receive1 = 1U;
__IO uint32_t  receive_length1 = 0U;

//usbd_int_cb_struct *usbd_int_fops = NULL;

typedef struct
{
    uint32_t dwDTERate;   /* data terminal rate */
    uint8_t  bCharFormat; /* stop bits */
    uint8_t  bParityType; /* parity */
    uint8_t  bDataBits;   /* data bits */
}line_coding_struct;

line_coding_struct linecoding =
{
    115200, /* baud rate     */
    0x00,   /* stop bits - 1 */
    0x00,   /* parity - none */
    0x08    /* num of bits 8 */
};

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_desc_dev device_descriptor_vcom_enable =
{
    .header = 
     {
         .bLength = USB_DEV_DESC_LEN, 
         .bDescriptorType = USB_DESCTYPE_DEV
     },
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xef,
    .bDeviceSubClass = 0x02,
    .bDeviceProtocol = 0x01,
    .bMaxPacketSize0 = USB_FS_EP0_MAX_LEN,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = STR_IDX_MFC,
    .iProduct = STR_IDX_PRODUCT,
    .iSerialNumber = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

const usb_desc_dev device_descriptor_vcom_disable =
{
    .header =
     {
         .bLength = USB_DEV_DESC_LEN,
         .bDescriptorType = USB_DESCTYPE_DEV
     },
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x02,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
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
const usb_descriptor_configuration_set_struct_vcom_disable configuration_descriptor_vcom_disable =
{
    .config =
    {
        .header =
         {
            .bLength = USB_CFG_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength = sizeof(usb_descriptor_configuration_set_struct_vcom_disable),
        .bNumInterfaces = 0x02 * USB_CDC_ACM_NUM,
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
        .iInterface = 0x00
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
const usb_descriptor_configuration_set_struct_vcom_enable configuration_descriptor_vcom_enable =
{
    .config = 
    {
        .header = 
         {
            .bLength = USB_CFG_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength = sizeof(usb_descriptor_configuration_set_struct_vcom_enable),
        .bNumInterfaces = 0x02 * USB_CDC_ACM_NUM,
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
         .iFunction = 0x00,
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
        .iInterface = 0x00
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
         .iFunction = 0x00,
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
        .iInterface = 0x00
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

void* const usbd_strings_vcom_disable[] =
{
    [STR_IDX_LANGID] = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC] = USBD_STRING_DESC("RV-LINK"),
    [STR_IDX_PRODUCT] = USBD_STRING_DESC(RVL_LINK_CONFIG_NAME),
    [STR_IDX_SERIAL] = USBD_STRING_DESC("GD32XXX-3.0.0-7z8x9yer")
};

void* const usbd_strings_vcom_enable[] =
{
    [STR_IDX_LANGID] = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC] = USBD_STRING_DESC("RV-LINK+VCOM"),
    [STR_IDX_PRODUCT] = USBD_STRING_DESC(RVL_LINK_CONFIG_NAME),
    [STR_IDX_SERIAL] = USBD_STRING_DESC("GD32XXX-3.0.0-7z8x9yer")
};

int rvl_vcom_enable(void);

/*!
    \brief      initialize the CDC ACM device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_init (usb_dev *pudev, uint8_t config_index)
{
    /* initialize the data Tx/Rx endpoint */
    if(rvl_vcom_enable()) {
        usbd_ep_setup(pudev, &(configuration_descriptor_vcom_enable.cdc[0].ep_data_in));
        usbd_ep_setup(pudev, &(configuration_descriptor_vcom_enable.cdc[0].ep_data_out));

        usbd_ep_setup(pudev, &(configuration_descriptor_vcom_enable.cdc[1].ep_data_in));
        usbd_ep_setup(pudev, &(configuration_descriptor_vcom_enable.cdc[1].ep_data_out));
    } else {
        usbd_ep_setup(pudev, &(configuration_descriptor_vcom_disable.cdc[0].ep_data_in));
        usbd_ep_setup(pudev, &(configuration_descriptor_vcom_disable.cdc[0].ep_data_out));
    }

    return USBD_OK;
}

/*!
    \brief      de-initialize the CDC ACM device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_deinit (usb_dev *pudev, uint8_t config_index)
{
    /* deinitialize the data Tx/Rx endpoint */
    usbd_ep_clear(pudev, CDC_ACM_DATA_IN_EP);
    usbd_ep_clear(pudev, CDC_ACM_DATA_OUT_EP);

    if(rvl_vcom_enable()) {
        usbd_ep_clear(pudev, CDC_ACM1_DATA_IN_EP);
        usbd_ep_clear(pudev, CDC_ACM1_DATA_OUT_EP);
    }

    return USBD_OK;
}

/*!
    \brief      handle CDC ACM data
    \param[in]  pudev: pointer to USB device instance
    \param[in]  rx_tx: data transfer direction:
      \arg        USBD_TX
      \arg        USBD_RX
    \param[in]  ep_id: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_data_out_handler (usb_dev *pudev, uint8_t ep_id)
{
    if ((EP0_OUT & 0x7F) == ep_id) 
    {
        cdc_acm_EP0_RxReady (pudev);
    } 
    else if ((CDC_ACM_DATA_OUT_EP & 0x7F) == ep_id)
    {
        packet_receive = 1;
        receive_length = usbd_rxcount_get(pudev, CDC_ACM_DATA_OUT_EP);

        return USBD_OK;
    }
    else if ((CDC_ACM1_DATA_OUT_EP & 0x7F) == ep_id)
    {
        packet_receive1 = 1;
        receive_length1 = usbd_rxcount_get(pudev, CDC_ACM1_DATA_OUT_EP);

        return USBD_OK;
    }

    return USBD_FAIL;
}

uint8_t cdc_acm_data_in_handler (usb_dev *pudev, uint8_t ep_id)
{
    if ((CDC_ACM_DATA_IN_EP & 0x7F) == ep_id)
    {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];

        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send (pudev, ep_id, NULL, 0U);
        } else {
            packet_sent = 1;
        }
        return USBD_OK;
    } 
    else if ((CDC_ACM1_DATA_IN_EP & 0x7F) == ep_id)
    {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];

        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send (pudev, ep_id, NULL, 0U);
        } else {
            packet_sent1 = 1;
        }
        return USBD_OK;
    }

    return USBD_FAIL;
}


/*!
    \brief      handle the CDC ACM class-specific requests
    \param[in]  pudev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_req_handler (usb_dev *pudev, usb_req *req)
{
    switch (req->bRequest) 
    {
        case SEND_ENCAPSULATED_COMMAND:
            break;
        case GET_ENCAPSULATED_RESPONSE:
            break;
        case SET_COMM_FEATURE:
            break;
        case GET_COMM_FEATURE:
            break;
        case CLEAR_COMM_FEATURE:
            break;
        case SET_LINE_CODING:
            /* set the value of the current command to be processed */
            if(req->wIndex == 0x02) {
                cdc_cmd = SET_LINE_CODING;
            }
            /* enable EP0 prepare to receive command data packet */
            pudev->dev.transc_out[0].xfer_buf = usb_cmd_buffer;
            pudev->dev.transc_out[0].remain_len = req->wLength;
            break;
        case GET_LINE_CODING:
            usb_cmd_buffer[0] = (uint8_t)(linecoding.dwDTERate);
            usb_cmd_buffer[1] = (uint8_t)(linecoding.dwDTERate >> 8);
            usb_cmd_buffer[2] = (uint8_t)(linecoding.dwDTERate >> 16);
            usb_cmd_buffer[3] = (uint8_t)(linecoding.dwDTERate >> 24);
            usb_cmd_buffer[4] = linecoding.bCharFormat;
            usb_cmd_buffer[5] = linecoding.bParityType;
            usb_cmd_buffer[6] = linecoding.bDataBits;
            /* send the request data to the host */
            pudev->dev.transc_in[0].xfer_buf = usb_cmd_buffer;
            pudev->dev.transc_in[0].remain_len = req->wLength;
            break;
        case SET_CONTROL_LINE_STATE:
            break;
        case SEND_BREAK:
            break;
        default:
            break;
    }

    return USBD_OK;
}

/*!
    \brief      receive CDC ACM data
    \param[in]  pudev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void cdc_acm_data_receive(usb_dev *pudev)
{
    packet_receive = 0;

    usbd_ep_recev(pudev, CDC_ACM_DATA_OUT_EP, (uint8_t*)(usb_data_buffer), CDC_ACM_DATA_PACKET_SIZE);
}

/*!
    \brief      send CDC ACM data
    \param[in]  pudev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void cdc_acm_data_send (usb_dev *pudev, uint32_t data_len)
{
    /* limit the transfer data length */
    if (data_len <= CDC_ACM_DATA_PACKET_SIZE) {
        packet_sent = 0;
        usbd_ep_send(pudev, CDC_ACM_DATA_IN_EP, (uint8_t*)(usb_data_buffer), data_len);
    }
}

/*!
    \brief      command data received on control endpoint
    \param[in]  pudev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_EP0_RxReady (usb_dev *pudev)
{
    if (SET_LINE_CODING == cdc_cmd) {
        /* process the command data */
        linecoding.dwDTERate = (uint32_t)(usb_cmd_buffer[0] | 
                                         (usb_cmd_buffer[1] << 8) |
                                         (usb_cmd_buffer[2] << 16) |
                                         (usb_cmd_buffer[3] << 24));

        linecoding.bCharFormat = usb_cmd_buffer[4];
        linecoding.bParityType = usb_cmd_buffer[5];
        linecoding.bDataBits = usb_cmd_buffer[6];

        cdc_cmd = NO_CMD;

        void serial1_set_line_coding(uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity);
        serial1_set_line_coding(linecoding.dwDTERate, linecoding.bDataBits, linecoding.bCharFormat, linecoding.bParityType);
    }

    return USBD_OK;
}


usb_class_core usbd_cdc_cb = {
    .command         = NO_CMD,
    .alter_set       = 0,

    .init            = cdc_acm_init,
    .deinit          = cdc_acm_deinit,
    .req_proc        = cdc_acm_req_handler,
    .data_in         = cdc_acm_data_in_handler,
    .data_out        = cdc_acm_data_out_handler
};


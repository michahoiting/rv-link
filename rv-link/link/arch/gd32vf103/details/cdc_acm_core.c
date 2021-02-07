/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *     Copyright (c) 2021, Micha Hoiting <micha.hoiting@gmail.com>
 *
 *     \file  rv-link/link/arch/gd32vf103/details/cdc_acm_core.c
 *     \brief CDC ACM driver.
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

/* system library header file includes */
#include <stdint.h>

/* own header file include */
#include <rv-link/link/arch/gd32vf103/details/cdc_acm_core.h>

/* other library header file includes */
#include "gd32vf103_soc_sdk.h"
#include "usbd_enum.h"

/* other project header file includes */
#include <rv-link/details/debug.h>

/* own component header file includes */
#include <rv-link/link/arch/gd32vf103/details/cdc_acm_descriptors.h>
#include <rv-link/link/details/link-config.h>
#include <rv-link/link/serial.h>

/* dependencies with external functions */
extern int rvl_vcom_enable(void);

#define USBD_VID 0x28e9
#define USBD_PID 0x018a

__IO uint8_t cdc_acm1_ep_packet_sent = 0U;
__IO uint8_t cdc_acm1_ep_packet_received = 0U;
__IO uint32_t cdc_acm1_ep_packet_length = 0U;

__IO uint8_t cdc_acm2_ep_packet_sent = 0U;
__IO uint8_t cdc_acm2_ep_packet_received = 0U;
__IO uint32_t cdc_acm2_ep_packet_length = 0U;

static uint32_t cdc_acm_core_cmd = NO_CMD;

static uint8_t usb_cmd_buffer[CDC_ACM_CMD_PACKET_SIZE];

typedef struct
{
    uint32_t dwDTERate;   /* data terminal rate */
    uint8_t  bCharFormat; /* stop bits */
    uint8_t  bParityType; /* parity */
    uint8_t  bDataBits;   /* data bits */
} line_coding_struct;

static line_coding_struct linecoding =
{
    115200, /* baud rate     */
    0x00,   /* stop bits - 1 */
    0x00,   /* parity - none */
    0x08    /* num of bits 8 */
};

/* command data received on control endpoint */
static uint8_t cdc_acm_core_ep0_data_out_handler(usb_dev  *pudev);

/*!
    \brief      initialize the CDC ACM device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_core_init(usb_dev *pudev, uint8_t config_index)
{
    /* initialize the data Tx/Rx endpoint */
    if (rvl_vcom_enable()) {
        usbd_ep_setup(pudev, (usb_desc_ep*) &(cdc_acm_configuration_descriptor_vcom_enable.cdc[0].ep_data_in));
        usbd_ep_setup(pudev, (usb_desc_ep*) &(cdc_acm_configuration_descriptor_vcom_enable.cdc[0].ep_data_out));

        usbd_ep_setup(pudev, (usb_desc_ep*) &(cdc_acm_configuration_descriptor_vcom_enable.cdc[1].ep_data_in));
        usbd_ep_setup(pudev, (usb_desc_ep*) &(cdc_acm_configuration_descriptor_vcom_enable.cdc[1].ep_data_out));
    } else {
        usbd_ep_setup(pudev, (usb_desc_ep*) &(cdc_acm_configuration_descriptor_vcom_disable.cdc[0].ep_data_in));
        usbd_ep_setup(pudev, (usb_desc_ep*) &(cdc_acm_configuration_descriptor_vcom_disable.cdc[0].ep_data_out));
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
static uint8_t cdc_acm_core_deinit(usb_dev *pudev, uint8_t config_index)
{
    /* deinitialize the data Tx/Rx endpoint */
    usbd_ep_clear(pudev, CDC_ACM1_DATA_IN_EP);
    usbd_ep_clear(pudev, CDC_ACM1_DATA_OUT_EP);

    if (rvl_vcom_enable()) {
        usbd_ep_clear(pudev, CDC_ACM2_DATA_IN_EP);
        usbd_ep_clear(pudev, CDC_ACM2_DATA_OUT_EP);
    }

    return USBD_OK;
}

/*!
    \brief      handle the CDC ACM class-specific requests
    \param[in]  pudev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_core_req_handler(usb_dev *pudev, usb_req *req)
{
    RVL_DEBUG_LOG(0, ("core_req bmReqType=%02x, bReq=%02x, wValue=%04x, wIndex=%04x, wLen=%04x\n",
            req->bmRequestType, req->bRequest, req->wValue, req->wIndex, req->wLength));

    switch (req->bRequest)
    {
        case SEND_ENCAPSULATED_COMMAND:
            RVL_DEBUG_LOG(0, ("req=SEND_ENCAPSULATED_COMMAND\n"));
            break;
        case GET_ENCAPSULATED_RESPONSE:
            RVL_DEBUG_LOG(0, ("req=GET_ENCAPSULATED_RESPONSE\n"));
            break;
        case SET_COMM_FEATURE:
            RVL_DEBUG_LOG(0, ("req=SET_COMM_FEATURE\n"));
            break;
        case GET_COMM_FEATURE:
            RVL_DEBUG_LOG(0, ("req=GET_COMM_FEATURE\n"));
            break;
        case CLEAR_COMM_FEATURE:
            RVL_DEBUG_LOG(0, ("req=CLEAR_COMM_FEATURE\n"));
            break;
        case SET_LINE_CODING:
        {
            const uint16_t interface = req->wIndex;
            /* bmRequestType = 00100001b */
            RVL_DEBUG_LOG(0, ("req=SET_LINE_CODING intf=%d\n", interface));
            /* Only change the settings of the VCOM interface */
            if (interface == 0x02) {
                //cdc_acm_core_cmd = SET_LINE_CODING;
            }
            /* enable EP0 prepare to receive command data packet */
            pudev->dev.transc_out[0].xfer_buf = usb_cmd_buffer;
            pudev->dev.transc_out[0].remain_len = req->wLength;
            break;
        }
        case GET_LINE_CODING:
            RVL_DEBUG_LOG(0, ("req=GET_LINE_CODING\n"));
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
        {
            const uint16_t interface = req->wIndex;
            const uint16_t ctrl_bmap = req->wValue;
            RVL_DEBUG_LOG(0, ("req=SET_CONTROL_LINE_STATE intf=%d, ctrl_bmap=%04x\n", interface, ctrl_bmap));
            if (ctrl_bmap & 0x01) { RVL_DEBUG_LOG(0, ("DTE is present, ")); } else { RVL_DEBUG_LOG(0, ("DTE is NOT present, ")); }
            if (ctrl_bmap & 0x02) { RVL_DEBUG_LOG(0, ("RTS is ON\n")); } else { RVL_DEBUG_LOG(0, ("RTS is OFF\n")); }
            break;
        }
        case SEND_BREAK:
            RVL_DEBUG_LOG(0, ("req=SEND_BREAK\n"));
            break;
        default:
            RVL_DEBUG_LOG(0, ("unhandled req=%x\n", req->bRequest));
            break;
    }
    return USBD_OK;
}

/*!
    \brief      handle incomming CDC ACM data from the host
    \param[in]  pudev: pointer to USB device instance
    \param[in]  ep_id: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_core_data_out_handler(usb_dev *pudev, uint8_t ep_id)
{
    RVL_DEBUG_LOG(0, ("data_out_handler ep_id=%x, xfer_len=%u\n", ep_id, usbd_rxcount_get(pudev, ep_id)));

    if (ep_id == (EP0_OUT & 0x7F)) {
        /* TODO return ? */ return cdc_acm_core_ep0_data_out_handler(pudev);
    } else if (ep_id == (CDC_ACM1_DATA_OUT_EP & 0x7F)) {
        cdc_acm1_ep_packet_received = 1;
        cdc_acm1_ep_packet_length = usbd_rxcount_get(pudev, CDC_ACM1_DATA_OUT_EP);
        return USBD_OK;
    } else if (ep_id == (CDC_ACM2_DATA_OUT_EP & 0x7F)) {
        cdc_acm2_ep_packet_received = 1;
        cdc_acm2_ep_packet_length = usbd_rxcount_get(pudev, CDC_ACM2_DATA_OUT_EP);
        return USBD_OK;
    }
    return USBD_FAIL;
}

/*!
    \brief      handle outgoing CDC ACM data requested by the host
    \param[in]  pudev: pointer to USB device instance
    \param[in]  ep_id: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_core_data_in_handler(usb_dev *pudev, uint8_t ep_id)
{
    RVL_DEBUG_LOG(0, ("data_in_handler ep_id=%d, ep_type=%02x, ep_stall=%02x, frame_num=%02x, xfer_len=%lx\n",
        ep_id,
        pudev->dev.transc_in[EP_ID(ep_id)].ep_type,
        pudev->dev.transc_in[EP_ID(ep_id)].ep_stall,
        pudev->dev.transc_in[EP_ID(ep_id)].frame_num,
        pudev->dev.transc_in[EP_ID(ep_id)].xfer_len));

    if (ep_id == (CDC_ACM1_DATA_IN_EP & 0x7F)) {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];
        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send(pudev, ep_id, NULL, 0U);
        } else {
            cdc_acm1_ep_packet_sent = 1;
        }
        return USBD_OK;
    } else if (ep_id == (CDC_ACM2_DATA_IN_EP & 0x7F)) {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];
        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send(pudev, ep_id, NULL, 0U);
        } else {
            cdc_acm2_ep_packet_sent = 1;
        }
        return USBD_OK;
    }
    return USBD_FAIL;
}

/*!
    \brief      handle incomming command data received on the EP0 control endpoint
    \param[in]  pudev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_core_ep0_data_out_handler(usb_dev *pudev)
{
    RVL_DEBUG_LOG(0, ("ep0_data_out_handler cmd=%lx\n", cdc_acm_core_cmd));
    if (cdc_acm_core_cmd == SET_LINE_CODING) {
        /* process the command data */
        linecoding.dwDTERate = (uint32_t)(usb_cmd_buffer[0] |
                                         (usb_cmd_buffer[1] << 8) |
                                         (usb_cmd_buffer[2] << 16) |
                                         (usb_cmd_buffer[3] << 24));
        linecoding.bCharFormat = usb_cmd_buffer[4];
        linecoding.bParityType = usb_cmd_buffer[5];
        linecoding.bDataBits = usb_cmd_buffer[6];

        cdc_acm_core_cmd = NO_CMD;

        rvl_serial_set_line_coding(linecoding.dwDTERate, linecoding.bDataBits, linecoding.bCharFormat, linecoding.bParityType);
        RVL_DEBUG_LOG(0, ("rvl_serial_set_line_coding %lu-%u-%u-%u\n", linecoding.dwDTERate, linecoding.bDataBits, linecoding.bCharFormat, linecoding.bParityType));
    }
    return USBD_OK;
}

void cdc_acm_core_init_desc(usb_desc *desc)
{
    /* TODO: make sure that gdb-server has been initialized */
    int vcom = rvl_vcom_enable();

    desc->dev_desc = vcom ? (uint8_t*) &cdc_acm_device_descriptor_vcom_enable:
                            (uint8_t*) &cdc_acm_device_descriptor_vcom_disable;

    desc->config_desc = vcom ? (uint8_t*) &cdc_acm_configuration_descriptor_vcom_enable:
                               (uint8_t*) &cdc_acm_configuration_descriptor_vcom_disable;

    desc->strings = cdc_acm_get_dev_strings_desc();
}

__IO usb_class_core cdc_acm_usb_class_core = {
    .command         = NO_CMD,
    .alter_set       = 0,
    .init            = cdc_acm_core_init,
    .deinit          = cdc_acm_core_deinit,
    .req_proc        = cdc_acm_core_req_handler,
    .data_in         = cdc_acm_core_data_in_handler,
    .data_out        = cdc_acm_core_data_out_handler
};

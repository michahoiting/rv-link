/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *
 *     \file  rv-link/link/arch/gd32vf103c/details/cdc_acm_core.c
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

#include <stdlib.h>
#include <string.h>

#include <rv-link/link/arch/gd32vf103c/details/cdc_acm_core.h>
#include <rv-link/link/details/link-config.h>
#include <rv-link/link/serial.h>

#define USBD_VID                          0x28e9
#define USBD_PID                          0x018a

static uint32_t cdc_cmd = 0xFFU;

static uint8_t usb_cmd_buffer[CDC_ACM_CMD_PACKET_SIZE];

__IO uint8_t cdc_acm0_packet_sent = 1U;
__IO uint8_t cdc_acm0_packet_received = 1U;
__IO uint32_t cdc_acm0_packet_length = 0U;

__IO uint8_t cdc_acm1_packet_sent = 1U;
__IO uint8_t cdc_acm1_packet_received = 1U;
__IO uint32_t cdc_acm1_packet_length = 0U;

typedef struct
{
    uint32_t dwDTERate;   /* data terminal rate */
    uint8_t  bCharFormat; /* stop bits */
    uint8_t  bParityType; /* parity */
    uint8_t  bDataBits;   /* data bits */
} line_coding_struct;

line_coding_struct linecoding =
{
    115200, /* baud rate     */
    0x00,   /* stop bits - 1 */
    0x00,   /* parity - none */
    0x08    /* num of bits 8 */
};

/* dependencies with external functions */
extern const void* cdc_acm_get_dev_strings_desc(void);
extern int rvl_vcom_enable(void);

/* command data received on control endpoint */
static uint8_t cdc_acm_EP0_RxReady(usb_dev  *pudev);


/*!
    \brief      initialize the CDC ACM device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_init(usb_dev *pudev, uint8_t config_index)
{

    /* initialize the data Tx/Rx endpoint */
    if (rvl_vcom_enable()) {
        usbd_ep_setup(pudev, &(cdc_acm_configuration_descriptor_vcom_enable.cdc[0].ep_data_in));
        usbd_ep_setup(pudev, &(cdc_acm_configuration_descriptor_vcom_enable.cdc[0].ep_data_out));

        usbd_ep_setup(pudev, &(cdc_acm_configuration_descriptor_vcom_enable.cdc[1].ep_data_in));
        usbd_ep_setup(pudev, &(cdc_acm_configuration_descriptor_vcom_enable.cdc[1].ep_data_out));
    } else {
        usbd_ep_setup(pudev, &(cdc_acm_configuration_descriptor_vcom_disable.cdc[0].ep_data_in));
        usbd_ep_setup(pudev, &(cdc_acm_configuration_descriptor_vcom_disable.cdc[0].ep_data_out));
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
static uint8_t cdc_acm_deinit(usb_dev *pudev, uint8_t config_index)
{
    /* deinitialize the data Tx/Rx endpoint */
    usbd_ep_clear(pudev, CDC_ACM_DATA_IN_EP);
    usbd_ep_clear(pudev, CDC_ACM_DATA_OUT_EP);

    if (rvl_vcom_enable()) {
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
static uint8_t cdc_acm_data_out_handler(usb_dev *pudev, uint8_t ep_id)
{
    if ((EP0_OUT & 0x7F) == ep_id)
    {
        cdc_acm_EP0_RxReady (pudev);
    }
    else if ((CDC_ACM_DATA_OUT_EP & 0x7F) == ep_id)
    {
        cdc_acm0_packet_received = 1;
        cdc_acm0_packet_length = usbd_rxcount_get(pudev, CDC_ACM_DATA_OUT_EP);

        return USBD_OK;
    }
    else if ((CDC_ACM1_DATA_OUT_EP & 0x7F) == ep_id)
    {
        cdc_acm1_packet_received = 1;
        cdc_acm1_packet_length = usbd_rxcount_get(pudev, CDC_ACM1_DATA_OUT_EP);

        return USBD_OK;
    }

    return USBD_FAIL;
}


static uint8_t cdc_acm_data_in_handler(usb_dev *pudev, uint8_t ep_id)
{
    if ((CDC_ACM_DATA_IN_EP & 0x7F) == ep_id)
    {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];

        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send (pudev, ep_id, NULL, 0U);
        } else {
            cdc_acm0_packet_sent = 1;
        }
        return USBD_OK;
    }
    else if ((CDC_ACM1_DATA_IN_EP & 0x7F) == ep_id)
    {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];

        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send (pudev, ep_id, NULL, 0U);
        } else {
            cdc_acm1_packet_sent = 1;
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
static uint8_t cdc_acm_req_handler(usb_dev *pudev, usb_req *req)
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
            if (req->wIndex == 0x02) {
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
    \brief      command data received on control endpoint
    \param[in]  pudev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t cdc_acm_EP0_RxReady(usb_dev *pudev)
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

        rvl_serial_set_line_coding(linecoding.dwDTERate, linecoding.bDataBits, linecoding.bCharFormat, linecoding.bParityType);
    }

    return USBD_OK;
}

void cdc_acm_init_desc(usb_desc *desc)
{
    /* TODO: make sure that gdb-server has been initialized */
    int vcom = rvl_vcom_enable();

    desc->dev_desc = (uint8_t*)&cdc_acm_device_descriptor;
    desc->config_desc = vcom ? (uint8_t *) &cdc_acm_configuration_descriptor_vcom_enable:
                               (uint8_t *) &cdc_acm_configuration_descriptor_vcom_disable;

    desc->strings = cdc_acm_get_dev_strings_desc();
}


usb_class_core cdc_acm_usb_class_core = {
    .command         = NO_CMD,
    .alter_set       = 0,
    .init            = cdc_acm_init,
    .deinit          = cdc_acm_deinit,
    .req_proc        = cdc_acm_req_handler,
    .data_in         = cdc_acm_data_in_handler,
    .data_out        = cdc_acm_data_out_handler
};

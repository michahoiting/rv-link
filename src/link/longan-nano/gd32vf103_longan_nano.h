/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *
 *     \file    gd32vf103_longan_nano.h
 *     \brief   Interface of firmware functions to manage leds, keys, UART ports
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

#ifndef __GD32VF103_LONGAN_NANO_H__
#define __GD32VF103_LONGAN_NANO_H__

#include <stdint.h>

#ifdef cplusplus
 extern "C" {
#endif

/*
 * Sipeed Longan Nano Developer Board
 *
 * SOC: GD32VF103CBT6
 *
 *   CPU: Bumblebee based on RISC-V 32-bit core with IMAC extension
 *   ROM: 128KB Flash @ 0x08000000 - 0x0801ffff
 *   RAM: 32KB SRAM   @ 0x20000000 - 0x20007fff
 *
 *   Built-in peripherals:
 *   - 4 x Universal 16-bit Timer
 *   - 2 x Basic 16-bit Timer
 *   - 1 x Advanced 16-bit Timer
 *   - Watchdog timer
 *   - RTC
 *   - Systick
 *   - 3 x USART
 *   - 2 x I2C
 *   - 3 x SPI
 *   - 2 x I2S
 *   - 2 x CAN
 *   - 1 x USBFS(OTG)
 *   - 2 x ADC(10 channel)
 *   - 2 x DAC
 *
 * Board features:
 * - 8MHz Passive High Speed Crystal
 * - 32.768KHz Low Speed RTC Crystal
 * - Storage expansion: Short body TF card slot (on SPI1)
 * - Display expansion: 8pin 0.5mm block expansion 160x80 RGB IPS LCD (on SPI0)
 * - Debug interface: 2x4 pin to JTAG debug interface
 * - RGB LED (on PC13, PA1, PA2)
 * - Reset button
 * - BOOT0 button (non documented: connected to PA8, high when pressed)
 */

/* exported types */
typedef enum
{
    LED_1 = 0, LED_RGB_R = LED_1,
    LED_2 = 1, LED_RGB_G = LED_2,
    LED_3 = 2, LED_RGB_B = LED_2,
    LED_PA4 = 3, /* additional connected led */
    LED_PA5 = 4, /* additional connected led */
    LED_PA6 = 5, /* additional connected led */
} led_typedef_enum;

typedef enum
{
    KEY_A = 0, KEY_BOOT0 = KEY_A,
} key_typedef_enum;

typedef enum
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} key_mode_typedef_enum;

typedef enum
{
    UART_0 = 0,
    UART_0_REMAP = 1,
    UART_1 = 2,
    UART_2 = 3,
} uart_typedef_enum;

typedef enum
{
    UART_INTMODE_ENABLE = 0,
    UART_INTMODE_DISABLE = 1,
} uart_intmode_typedef_enum;

/* function declarations */

/* configure led */
void gd_longan_nano_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_longan_nano_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_longan_nano_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_longan_nano_led_toggle(led_typedef_enum lednum);

/* configure key */
void gd_longan_nano_key_init(key_typedef_enum keynum, key_mode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_longan_nano_key_state_get(key_typedef_enum keynum);

/* configure UART */
void gd_longan_nano_uart_init(uart_typedef_enum uartnum, uart_intmode_typedef_enum uart_intmode);
void gd_longan_nano_uart_set_line_coding(uart_typedef_enum uartnum, uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity);

#ifdef cplusplus
}
#endif

#endif /* __GD32VF103_LONGAN_NANO_H__ */

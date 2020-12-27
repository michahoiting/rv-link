/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *
 *     \file    gd32vf103_longan_nano_internal.h
 *     \brief   Definitions for GD32VF103 Sipeed Longan Nano's leds, keys and UART
 *              ports hardware resources
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

#ifndef __GD32VF103_LONGAN_NANO_INTERNAL_H__
#define __GD32VF103_LONGAN_NANO_INTERNAL_H__

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32vf103.h"

#define LEDn                             6U

/* RGB LED Red */
#define LED_1_PIN                        GPIO_PIN_13
#define LED_1_GPIO_PORT                  GPIOC
#define LED_1_GPIO_CLK                   RCU_GPIOC

/* RGB LED Green */
#define LED_2_PIN                        GPIO_PIN_1
#define LED_2_GPIO_PORT                  GPIOA
#define LED_2_GPIO_CLK                   RCU_GPIOA

/* RGB LED Blue */
#define LED_3_PIN                        GPIO_PIN_2
#define LED_3_GPIO_PORT                  GPIOA
#define LED_3_GPIO_CLK                   RCU_GPIOA

/* Extra LED on A4 */
#define LED_PA4_PIN                      GPIO_PIN_4
#define LED_PA4_GPIO_PORT                GPIOA
#define LED_PA4_GPIO_CLK                 RCU_GPIOA

/* Extra LED on A5 */
#define LED_PA5_PIN                      GPIO_PIN_5
#define LED_PA5_GPIO_PORT                GPIOA
#define LED_PA5_GPIO_CLK                 RCU_GPIOA

/* Extra LED on A6 */
#define LED_PA6_PIN                      GPIO_PIN_6
#define LED_PA6_GPIO_PORT                GPIOA
#define LED_PA6_GPIO_CLK                 RCU_GPIOA

#define KEYn                             1U

/* BOOT0 push-button (non documented, but goes high when pressed) */
#define KEY_BOOT0_PIN                    GPIO_PIN_8
#define KEY_BOOT0_GPIO_PORT              GPIOA
#define KEY_BOOT0_GPIO_CLK               RCU_GPIOA
#define KEY_BOOT0_EXTI_LINE              EXTI_8
#define KEY_BOOT0_EXTI_PORT_SOURCE       GPIO_PORT_SOURCE_GPIOA
#define KEY_BOOT0_EXTI_PIN_SOURCE        GPIO_PIN_SOURCE_8
#define KEY_BOOT0_EXTI_IRQn              EXTI10_15_IRQn

#define UARTn                            4U

#define UART_0_ITF                       USART0
#define UART_0_TX_PIN                    GPIO_PIN_9
#define UART_0_RX_PIN                    GPIO_PIN_10
#define UART_0_GPIO_PORT                 GPIOA
#define UART_0_GPIO_CLK                  RCU_GPIOA
#define UART_0_GPIO_REMAP                0
#define UART_0_CLK                       RCU_USART0
#define UART_0_IRQ                       USART0_IRQn

#define UART_0_REMAP_ITF                 USART0
#define UART_0_REMAP_TX_PIN              GPIO_PIN_6
#define UART_0_REMAP_RX_PIN              GPIO_PIN_7
#define UART_0_REMAP_GPIO_PORT           GPIOB
#define UART_0_REMAP_GPIO_CLK            RCU_GPIOB
#define UART_0_REMAP_GPIO_REMAP          GPIO_USART0_REMAP
#define UART_0_REMAP_CLK                 RCU_USART0
#define UART_0_REMAP_IRQ                 USART0_IRQn

#define UART_1_ITF                       USART1
#define UART_1_TX_PIN                    GPIO_PIN_2
#define UART_1_RX_PIN                    GPIO_PIN_3
#define UART_1_GPIO_PORT                 GPIOA
#define UART_1_GPIO_CLK                  RCU_GPIOA
#define UART_1_GPIO_REMAP                0
#define UART_1_CLK                       RCU_USART1
#define UART_1_IRQ                       USART1_IRQn

#define UART_2_ITF                       USART2
#define UART_2_TX_PIN                    GPIO_PIN_10
#define UART_2_RX_PIN                    GPIO_PIN_11
#define UART_2_GPIO_PORT                 GPIOB
#define UART_2_GPIO_CLK                  RCU_GPIOB
#define UART_2_GPIO_REMAP                0
#define UART_2_CLK                       RCU_USART2
#define UART_2_IRQ                       USART2_IRQn

#ifdef cplusplus
}
#endif

#endif /* __GD32VF103_LONGAN_NANO_INTERNAL_H__ */

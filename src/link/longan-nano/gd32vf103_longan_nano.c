/**
 *     Copyright (c) 2019, GigaDevice Semiconductor Inc.
 *
 *     \file    gd32vf103_longan_nano.c
 *     \brief   Firmware functions to manage leds, keys, UART ports
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

#include "gd32vf103_longan_nano.h"
#include "gd32vf103_longan_nano_internal.h"
#include "riscv_encoding.h"

/* private variables */
static const uint32_t GPIO_PIN[LEDn] = {
    LED_1_PIN, LED_2_PIN, LED_3_PIN,
    LED_PA4_PIN, LED_PA5_PIN, LED_PA6_PIN};
static const uint32_t GPIO_PORT[LEDn] = {
    LED_1_GPIO_PORT, LED_2_GPIO_PORT, LED_3_GPIO_PORT,
    LED_PA4_GPIO_PORT, LED_PA5_GPIO_PORT, LED_PA6_GPIO_PORT};
static const rcu_periph_enum GPIO_CLK[LEDn] = {
    LED_1_GPIO_CLK, LED_2_GPIO_CLK, LED_3_GPIO_CLK,
    LED_PA4_GPIO_CLK, LED_PA5_GPIO_CLK, LED_PA6_GPIO_CLK};

static const uint32_t UART_ITF[UARTn] = {
    UART_0_ITF, UART_0_REMAP_ITF, UART_1_ITF, UART_2_ITF};
static const uint32_t UART_TX_PIN[UARTn] = {
    UART_0_TX_PIN, UART_0_REMAP_TX_PIN, UART_1_TX_PIN, UART_2_TX_PIN};
static const uint32_t UART_RX_PIN[UARTn] = {
    UART_0_RX_PIN, UART_0_REMAP_RX_PIN, UART_1_RX_PIN, UART_2_RX_PIN};
static const uint32_t UART_GPIO_PORT[UARTn] = {
    UART_0_GPIO_PORT, UART_0_REMAP_GPIO_PORT, UART_1_GPIO_PORT, UART_2_GPIO_PORT};
static const rcu_periph_enum UART_GPIO_CLK[UARTn] = {
    UART_0_GPIO_CLK, UART_0_REMAP_GPIO_CLK, UART_1_GPIO_CLK, UART_2_GPIO_CLK};
static const rcu_periph_enum UART_CLK[UARTn] = {
    UART_0_CLK, UART_0_REMAP_CLK, UART_1_CLK, UART_2_CLK};
static const IRQn_Type UART_IRQ[UARTn] = {
    UART_0_IRQ, UART_0_REMAP_IRQ, UART_1_IRQ, UART_2_IRQ};
static const uint32_t UART_REMAP[UARTn] = {
    UART_0_GPIO_REMAP, UART_0_REMAP_GPIO_REMAP, UART_1_GPIO_REMAP, UART_2_GPIO_REMAP};

static const uint32_t KEY_PIN[KEYn] = {KEY_BOOT0_PIN};
static const uint32_t KEY_PORT[KEYn] = {KEY_BOOT0_GPIO_PORT};
static const rcu_periph_enum KEY_CLK[KEYn] = {KEY_BOOT0_GPIO_CLK};
static const exti_line_enum KEY_EXTI_LINE[KEYn] = {KEY_BOOT0_EXTI_LINE};
static const uint8_t KEY_PORT_SOURCE[KEYn] = {KEY_BOOT0_EXTI_PORT_SOURCE};
static const uint8_t KEY_PIN_SOURCE[KEYn] = {KEY_BOOT0_EXTI_PIN_SOURCE};
static const uint8_t KEY_IRQn[KEYn] = {KEY_BOOT0_EXTI_IRQn};

/*!
    \brief      configure led GPIO
    \param[in]  lednum: specify the led to be configured
      \arg        LED_1
      \arg        LED_2
      \arg        LED_3
    \param[out] none
    \retval     none
*/
void gd_longan_nano_led_init(led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
    /* configure led GPIO port */
    gpio_init(GPIO_PORT[lednum], GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN[lednum]);

    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn on selected led
    \param[in]  lednum: specify the led to be turned on
      \arg        LED_1
      \arg        LED_2
      \arg        LED_3
    \param[out] none
    \retval     none
*/
void gd_longan_nano_led_on(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn off selected led
    \param[in]  lednum: specify the led to be turned off
      \arg        LED_1
      \arg        LED_2
      \arg        LED_3
    \param[out] none
    \retval     none
*/
void gd_longan_nano_led_off(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      toggle selected led
    \param[in]  lednum: specify the led to be toggled
      \arg        LED_1
      \arg        LED_2
      \arg        LED_3
    \param[out] none
    \retval     none
*/
void gd_longan_nano_led_toggle(led_typedef_enum lednum)
{
    gpio_bit_write(GPIO_PORT[lednum], GPIO_PIN[lednum],
        (bit_status)(1-gpio_input_bit_get(GPIO_PORT[lednum], GPIO_PIN[lednum])));
}

/*!
    \brief      configure key
    \param[in]  keynum: specify the key to be configured
      \arg        KEY_A: BOOT0 key
    \param[in]  key_mode: specify button mode
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_longan_nano_key_init(key_typedef_enum keynum, key_mode_typedef_enum key_mode)
{
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[keynum]);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin as input */
    gpio_init(KEY_PORT[keynum], GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, KEY_PIN[keynum]);

    if (key_mode == KEY_MODE_EXTI) {
        /* enable and set key EXTI interrupt to the lowest priority */
        eclic_irq_enable(KEY_IRQn[keynum],1, 1);
        /* set machine interrupt enable bit */
        set_csr(mstatus, MSTATUS_MIE);

        /* connect key EXTI line to key GPIO pin */
        gpio_exti_source_select(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);

        /* configure key EXTI line */
        exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
    }
}

/*!
    \brief      return the selected key state
    \param[in]  keynum: specify the key to be checked
      \arg        KEY_A: BOOT0 key
    \param[out] none
    \retval     the key's GPIO pin value
*/
uint8_t gd_longan_nano_key_state_get(key_typedef_enum keynum)
{
    return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
}

/*!
    \brief      configure UART port
    \param[in]  uartnum: UART on the board
      \arg        UART_0: USART0 (PA9, PA10) on the board
      \arg        UART_0_REMAP: USART0 (PB6, PB7) on the board
      \arg        UART_1: USART1 (PA2, PA3) on the board
      \arg        UART_2: USART2 (PB10, PB11) on the board
    \param[out] none
    \retval     none
*/
void gd_longan_nano_uart_init(uart_typedef_enum uartnum, uart_intmode_typedef_enum uart_intmode)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(UART_GPIO_CLK[uartnum]);

    /* enable USART clock */
    rcu_periph_clock_enable(UART_CLK[uartnum]);

    if (UART_REMAP[uartnum]) {
        gpio_pin_remap_config(UART_REMAP[uartnum], ENABLE);
    }

    /* connect port to USARTx_Tx */
    gpio_init(UART_GPIO_PORT[uartnum], GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, UART_TX_PIN[uartnum]);

    /* connect port to USARTx_Rx */
    gpio_init(UART_GPIO_PORT[uartnum], GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, UART_RX_PIN[uartnum]);

    /* USART configure */
    usart_deinit(UART_ITF[uartnum]);
    usart_baudrate_set(UART_ITF[uartnum], 115200U);
    usart_word_length_set(UART_ITF[uartnum], USART_WL_8BIT);
    usart_stop_bit_set(UART_ITF[uartnum], USART_STB_1BIT);
    usart_parity_config(UART_ITF[uartnum], USART_PM_NONE);
    usart_hardware_flow_rts_config(UART_ITF[uartnum], USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART_ITF[uartnum], USART_CTS_DISABLE);
    usart_receive_config(UART_ITF[uartnum], USART_RECEIVE_ENABLE);
    usart_transmit_config(UART_ITF[uartnum], USART_TRANSMIT_ENABLE);
    usart_enable(UART_ITF[uartnum]);

    /* enable USART receive interrupt */
    if (uart_intmode == UART_INTMODE_ENABLE) {
        usart_interrupt_enable(UART_ITF[uartnum], USART_INT_RBNE);
        eclic_irq_enable(UART_IRQ[uartnum], 1, 0);
    }
}


/*!
    \brief      configure UART port
    \param[in]  uartnum: UART on the board
      \arg        UART_0: USART0 (PA9, PA10) on the board
      \arg        UART_0_REMAP: USART0 (PB6, PB7) on the board
      \arg        UART_1: USART1 (PA2, PA3) on the board
      \arg        UART_2: USART2 (PB10, PB11) on the board
    \param[in]  baudrate: baud rate value
    \param[in]  data_bits: UART word length configure
                only one parameter can be selected which is shown as below:
      \arg        USART_WL_8BIT: 8 bits
      \arg        USART_WL_9BIT: 9 bits
    \param[in]  stop_bits: UART stop bit configure
                only one parameter can be selected which is shown as below:
      \arg        USART_STB_1BIT:   1 bit
      \arg        USART_STB_0_5BIT: 0.5 bit, not available for UARTx(x=3,4)
      \arg        USART_STB_2BIT:   2 bits
      \arg        USART_STB_1_5BIT: 1.5 bits, not available for UARTx(x=3,4)
    \param[in]  parity: configure UART parity
                only one parameter can be selected which is shown as below:
      \arg        USART_PM_NONE: no parity
      \arg        USART_PM_ODD:  odd parity
      \arg        USART_PM_EVEN: even parity
    \param[out] none
    \retval     none
*/
void gd_longan_nano_uart_set_line_coding(uart_typedef_enum uartnum, uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity)
{
    usart_disable(UART_ITF[uartnum]);
    usart_baudrate_set(UART_ITF[uartnum], baudrate);
    usart_word_length_set(UART_ITF[uartnum], data_bits);
    usart_stop_bit_set(UART_ITF[uartnum], stop_bits);
    usart_parity_config(UART_ITF[uartnum], parity);
    usart_enable(UART_ITF[uartnum]);
}

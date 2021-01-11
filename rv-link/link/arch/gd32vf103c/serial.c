/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/link/arch/gd32vf103c/serial.c
 * \brief Handling of the serial UART hardware to the target machine.
 *
 * RV-LINK is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

/* own header file include */
#include <rv-link/link/serial.h>

/* system library header file includes */
#include <stdbool.h>

/* other library header file includes */
#include <gd32vf103-sdk/GD32VF103_standard_peripheral/gd32vf103.h>
#include <pt/pt.h>

/* own component header file includes */
#include <rv-link/link/led.h>


#define RVL_SERIAL_BUFFER_SIZE 64

#if defined(RVL_SERIAL_USING_UART0_NO_REMAP)
    #define UART_ITF                       USART0
    #define UART_TX_PIN                    GPIO_PIN_9
    #define UART_RX_PIN                    GPIO_PIN_10
    #define UART_GPIO_PORT                 GPIOA
    #define UART_GPIO_CLK                  RCU_GPIOA
    #undef UART_GPIO_REMAP
    #define UART_CLK                       RCU_USART0
    #define UART_IRQ                       USART0_IRQn
#elif defined(RVL_SERIAL_USING_UART0_REMAP)
    #define UART_ITF                       USART0
    #define UART_TX_PIN                    GPIO_PIN_6
    #define UART_RX_PIN                    GPIO_PIN_7
    #define UART_GPIO_PORT                 GPIOB
    #define UART_GPIO_CLK                  RCU_GPIOB
    #define UART_GPIO_REMAP                GPIO_USART0_REMAP
    #define UART_CLK                       RCU_USART0
    #define UART_IRQ                       USART0_IRQn
#elif defined(RVL_SERIAL_USING_UART1_NO_REMAP)
    #define UART_ITF                       USART1
    #define UART_TX_PIN                    GPIO_PIN_2
    #define UART_RX_PIN                    GPIO_PIN_3
    #define UART_GPIO_PORT                 GPIOA
    #define UART_GPIO_CLK                  RCU_GPIOA
    #undef UART_GPIO_REMAP
    #define UART_CLK                       RCU_USART0
    #define UART_IRQ                       USART1_IRQn
#elif defined(RVL_SERIAL_USING_UART1_REMAP)
    /* Remap available only for 100-pin */
    #define UART_ITF                       USART1
    #define UART_TX_PIN                    GPIO_PIN_5
    #define UART_RX_PIN                    GPIO_PIN_6
    #define UART_GPIO_PORT                 GPIOD
    #define UART_GPIO_CLK                  RCU_GPIOD
    #define UART_GPIO_REMAP                GPIO_USART1_REMAP
    #define UART_CLK                       RCU_USART1
    #define UART_IRQ                       USART1_IRQn
#elif defined(RVL_SERIAL_USING_UART2_NO_REMAP)
    /* Remap available for 48-pin, 64-pin, 100-pin */
    #define UART_ITF                       USART2
    #define UART_TX_PIN                    GPIO_PIN_10
    #define UART_RX_PIN                    GPIO_PIN_11
    #define UART_GPIO_PORT                 GPIOB
    #define UART_GPIO_CLK                  RCU_GPIOB
    #undef UART_GPIO_REMAP
    #define UART_CLK                       RCU_USART2
    #define UART_IRQ                       USART2_IRQn
#elif defined(RVL_SERIAL_USING_UART2_PARTIAL_REMAP)
    /* Remap available only for 64-pin, 100-pin */
    #define UART_ITF                       USART2
    #define UART_TX_PIN                    GPIO_PIN_10
    #define UART_RX_PIN                    GPIO_PIN_11
    #define UART_GPIO_PORT                 GPIOC
    #define UART_GPIO_CLK                  RCU_GPIOC
    #define UART_GPIO_REMAP                GPIO_USART2_PARTIAL_REMAP
    #define UART_CLK                       RCU_USART2
    #define UART_IRQ                       USART2_IRQn
#elif defined(RVL_SERIAL_USING_UART2_FULL_REMAP)
    /* Remap available only for 100-pin */
    #define UART_ITF                       USART2
    #define UART_TX_PIN                    GPIO_PIN_8
    #define UART_RX_PIN                    GPIO_PIN_9
    #define UART_GPIO_PORT                 GPIOD
    #define UART_GPIO_CLK                  RCU_GPIOD
    #define UART_GPIO_REMAP                GPIO_USART2_FULL_REMAP
    #define UART_CLK                       RCU_USART2
    #define UART_IRQ                       USART2_IRQn
#else
#error No RVL_SERIAL_USING_USARTx defined
#endif

typedef struct buffer_s
{
    uint8_t buffer[RVL_SERIAL_BUFFER_SIZE];
    int head;
    int tail;
} buffer_t;

typedef struct rvl_serial_s
{
    struct pt pt_recv;
    struct pt pt_send;
    buffer_t recv_buf;
} rvl_serial_s;

static rvl_serial_s rvl_serial_i;
#define self rvl_serial_i

static void rvl_serial_recv_buf_put(uint8_t c);


void rvl_serial_init(void)
{
    PT_INIT(&self.pt_recv);
    PT_INIT(&self.pt_send);

    self.recv_buf.tail = 0;
    self.recv_buf.head = 0;

    /* enable GPIO clock */
    rcu_periph_clock_enable(UART_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(UART_CLK);

    rcu_periph_clock_enable(RCU_AF);

#ifdef UART_GPIO_REMAP
    /* enable USART GPIO remapping */
    gpio_pin_remap_config(UART_GPIO_REMAP, ENABLE);
#endif

    /* connect port to USARTx_Tx */
    gpio_init(UART_GPIO_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_10MHZ, UART_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(UART_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, UART_RX_PIN);

    /* USART configure */
    usart_deinit(UART_ITF);
    usart_baudrate_set(UART_ITF, 115200U);
    usart_word_length_set(UART_ITF, USART_WL_8BIT);
    usart_stop_bit_set(UART_ITF, USART_STB_1BIT);
    usart_parity_config(UART_ITF, USART_PM_NONE);
    usart_hardware_flow_rts_config(UART_ITF, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART_ITF, USART_CTS_DISABLE);
    usart_receive_config(UART_ITF, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART_ITF, USART_TRANSMIT_ENABLE);
    usart_enable(UART_ITF);

    /* enable USART receive interrupt */
    usart_interrupt_enable(UART_ITF, USART_INT_RBNE);
    eclic_irq_enable(UART_IRQ, 1, ECLIC_PRIGROUP_LEVEL2_PRIO2);
}


void rvl_serial_set_line_coding(uint32_t baudrate, uint32_t data_bits, uint32_t stop_bits, uint32_t parity)
{
    usart_disable(UART_ITF);
    usart_baudrate_set(UART_ITF, baudrate);
    /* TODO implement setting of data_bits,  stop_bits, and parity */
    usart_enable(UART_ITF);
}


PT_THREAD(rvl_serial_putchar(uint8_t c))
{
    PT_BEGIN(&self.pt_send);

    PT_WAIT_UNTIL(&self.pt_send, usart_flag_get(UART_ITF, USART_FLAG_TBE));

    usart_data_transmit(UART_ITF, c);
    rvl_led_indicator(RVL_LED_INDICATOR_LINK_SER_TX, true);

    PT_END(&self.pt_send);
}


PT_THREAD(rvl_serial_getchar(uint8_t *c))
{
    int tail;

    PT_BEGIN(&self.pt_recv);

    PT_WAIT_WHILE(&self.pt_recv, self.recv_buf.tail == self.recv_buf.head);

    tail = self.recv_buf.tail + 1;
    tail = tail >= sizeof(self.recv_buf.buffer) ? 0 : tail;
    *c = self.recv_buf.buffer[self.recv_buf.tail];
    asm volatile ("":::"memory");
    self.recv_buf.tail = tail;

    PT_END(&self.pt_recv);
}


static void rvl_serial_recv_buf_put(uint8_t c)
{
    int head = self.recv_buf.head + 1;
    head = head >= sizeof(self.recv_buf.buffer) ? 0 : head;
    if (self.recv_buf.tail != head) {
        self.recv_buf.buffer[self.recv_buf.head] = c;
        asm volatile ("":::"memory");
        self.recv_buf.head = head;
    }
}


#if defined(RVL_SERIAL_USING_UART0_NO_REMAP) || defined(RVL_SERIAL_USING_UART0_REMAP)
void USART0_IRQHandler(void)
#elif defined(RVL_SERIAL_USING_UART1_NO_REMAP) || defined(RVL_SERIAL_USING_UART1_REMAP)
void USART1_IRQHandler(void)
#elif defined(RVL_SERIAL_USING_UART2_NO_REMAP) || \
      defined(RVL_SERIAL_USING_UART2_PARTIAL_REMAP) || \
      defined(RVL_SERIAL_USING_UART2_FULL_REMAP)
void USART2_IRQHandler(void)
#else
#errro No RVL_SERIAL_USING_USARTx defined
#endif
{
    uint8_t c;
    if (usart_interrupt_flag_get(UART_ITF, USART_INT_FLAG_RBNE) != RESET) {
        /* read one byte from the receive data register */
        c = (uint8_t)usart_data_receive(UART_ITF);
        rvl_serial_recv_buf_put(c);
        rvl_led_indicator(RVL_LED_INDICATOR_LINK_SER_RX, true);
    }
}

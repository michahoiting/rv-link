/**
 *     Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 *     \file  rv-link/link/arch/gd32vf103/can.c
 *     \brief RV-Link CAN bus handling
 */

/* own header file include */
#include <rv-link/link/can.h>

/* other library header file includes */
#include "nuclei_sdk_soc.h"
#include "core_feature_eclic.h"

/* own component header file includes */
#include <rv-link/link/led.h>

/* select can */
//#define CAN0_USED
#define CAN1_USED

#ifdef CAN0_USED
#define CANX CAN0
#else
#define CANX CAN1
#endif

#ifndef CAN_BAUDRATE
/* select CAN baudrate */
/* 1MBps */
/* #define CAN_BAUDRATE  1000 */
/* 500kBps */
/* #define CAN_BAUDRATE  500 */
/* 250kBps */
#define CAN_BAUDRATE  250
/* 125kBps */
/* #define CAN_BAUDRATE  125 */
/* 100kBps */
/* #define CAN_BAUDRATE  100 */
/* 50kBps */
/* #define CAN_BAUDRATE  50 */
/* 20kBps */
/* #define CAN_BAUDRATE  20 */
#endif

int CANX_TX_IRQHandler_count = 0;
int CANX_RX0_IRQHandler_count = 0;
int CANX_RX1_IRQHandler_count = 0;

can_receive_message_struct can_fifo0_receive_message = {0};
can_receive_message_struct can_fifo1_receive_message = {0};

/* CAN0 interrupt handlers */
void CAN0_TX_IRQHandler(void);
void CAN0_RX0_IRQHandler(void);
void CAN0_RX1_IRQHandler(void);

/* CAN1 interrupt handlers */
void CAN1_TX_IRQHandler(void);
void CAN1_RX0_IRQHandler(void);
void CAN1_RX1_IRQHandler(void);

static void can_eclic_config(void);
static void can_gpio_config(void);
static void can_networking_init(void);
static void can_message_transmit_color(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rvl_can_init(void)
{
    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &can_fifo0_receive_message);
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &can_fifo1_receive_message);

     /* configure GPIO */
    can_gpio_config();

   /* initialize CAN */
    can_networking_init();

    /* configure CLIC
     *
     * IMPORTANT: must be configured AFTER setting the GPIO and CAN configuratation !
     */
    can_eclic_config();
}

/*!
    \brief      initialize CAN and filter
    \param[in]  can_parameter
      \arg        can_parameter_struct
    \param[in]  can_filter
      \arg        can_filter_parameter_struct
    \param[out] none
    \retval     none
*/
static void can_networking_init(void)
{
    can_parameter_struct can_parameter = {0};
    can_filter_parameter_struct can_filter = {0};

    /* initialize CAN register */
    can_deinit(CANX);

    /* initialize CAN */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.no_auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;

#if CAN_BAUDRATE == 1000
    can_parameter.prescaler = // 6 @ 108MHz;
    /* 500KBps */
#elif CAN_BAUDRATE == 500
    can_parameter.prescaler = // 12 @ 108MHz;
    /* 250KBps */
#elif CAN_BAUDRATE == 250
    can_parameter.prescaler = 21;
    /* 125KBps */
#elif CAN_BAUDRATE == 125
    can_parameter.prescaler = // 48 @ 108MHz;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100
    can_parameter.prescaler = // 60 @ 108MHz;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50
    can_parameter.prescaler = // 120 @ 108MHz;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20
    can_parameter.prescaler = // 300 @ 108MHz;
#else
    #error "please select list can baudrate in private defines in main.c"
#endif

    can_init(CANX, &can_parameter);

    /* initialize filter */
    can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);
#ifdef  CAN0_USED
    /* CAN0 filter number */
    can_filter.filter_number = 0;
#else
    /* CAN1 filter number */
    can_filter.filter_number = 15;
#endif
    /* initialize filter */
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;
    can_filter.filter_fifo_number = CAN_FIFO1;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void can_eclic_config(void)
{
    ECLIC_SetCfgNlbits(2);

    /* Globally enable interrupt servicing */
    __enable_irq();

#if defined(CAN0_USED)
    /* configure CAN0 CLIC FIFO0 */
    (void) ECLIC_Register_IRQ(CAN0_RX0_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);

    /* configure CAN0 CLIC FIFO1 */
    (void) ECLIC_Register_IRQ(CAN0_RX1_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);

//    (void) ECLIC_Register_IRQ(CAN0_TX_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
//    ECLIC_EnableIRQ(CAN0_TX_IRQn);
#elif defined(CAN1_USED)
    /* configure CAN1 CLIC FIFO0 */
    (void) ECLIC_Register_IRQ(CAN1_RX0_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
//    ECLIC_EnableIRQ(CAN1_RX0_IRQn);

    /* configure CAN1 CLIC FIFO1 */
    (void) ECLIC_Register_IRQ(CAN1_RX1_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
//    ECLIC_EnableIRQ(CAN1_RX1_IRQn);

//    (void) ECLIC_Register_IRQ(CAN1_TX_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
//    ECLIC_EnableIRQ(CAN1_TX_IRQn);
#else
#error No CANx_USED defined
#endif

    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CANX, CAN_INT_RFNE0);

    /* enable can receive FIFO1 not empty interrupt */
    can_interrupt_enable(CANX, CAN_INT_RFNE1);

    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);

    /* enable can receive FIFO1 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE1);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void can_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_AF);

    /* enable BOTH CAN clocks */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);

#if defined(CAN0_USED)
//    #define CAN0_NO_REMAP /* A11/A12 */
    #define CAN0_PARTIAL_REMAP /* B8/B9 */
//    #define CAN0_FULL_REMAP /* D0/D1 */
#elif defined(CAN1_USED)
    #define CAN1_NO_REMAP /* B12/B13 */
//    #define CAN1_REMAP /* B5/B6 */
#else
#error No CANx_USED defined
#endif

    /*********************************************************************************************
     * CAN0
     */

#ifdef CAN0_NO_REMAP
    /* configure CAN0 GPIO NO remap: A11/A12 */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11); /* A11: CAN0_RX - WHITE - PIN-4 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12); /* A12: CAN0_TX - YELLOW - PIN-1 */
#endif

#ifdef CAN0_PARTIAL_REMAP
    /* configure CAN0 GPIO GPIO_CAN0_PARTIAL_REMAP: B8/B9 */
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8); /* B8: CAN0_RX - WHITE - PIN-4 */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9); /* B9: CAN0_TX - YELLOW - PIN-1 */
    gpio_pin_remap_config(GPIO_CAN0_PARTIAL_REMAP, ENABLE);
#endif

#ifdef CAN0_FULL_REMAP
    /*
     * NOTE: THE PORTS GPIOD and GPIOE ARE NOT AVAILABLE ON THE GD32VF103CBT6 LQFP48 PACKAGE !!!
     */
    /* configure CAN0 GPIO GPIO_CAN0_FULL_REMAP: D0/D1 */
    rcu_periph_clock_enable(RCU_GPIOD);
    gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0); /* D0: CAN0_RX - WHITE - PIN-4 */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1); /* D1: CAN0_TX - YELLOW - PIN-1 */
    gpio_pin_remap_config(GPIO_CAN0_FULL_REMAP, ENABLE);
#endif

    /*********************************************************************************************
     * CAN1
     */

#ifdef CAN1_NO_REMAP
    /* configure CAN1 GPIO NO remap: B12/B13 */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_12); /* B12: CAN1_RX - WHITE - MCP-2551-PIN-4 */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_13); /* B13: CAN1_TX - YELLOW - MCP-2551-PIN-1 */
#endif

#ifdef CAN1_REMAP
    /* configure CAN1 GPIO GPIO_CAN1_REMAP: B5/B6 */
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5); /* B5: CAN1_RX - WHITE - PIN-4 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6); /* B6: CAN1_TX - YELLOW - PIN-1 */
    gpio_pin_remap_config(GPIO_CAN1_REMAP, ENABLE);
#endif
}


static void can_message_transmit_color(void)
{
    static int color = 0;
    can_trasnmit_message_struct can_transmit_message = {0};

    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &can_transmit_message);
    can_transmit_message.tx_sfid = 0x321U; /* standard format frame identifier */
    can_transmit_message.tx_efid = 0x00; /* extended format frame identifier */
    can_transmit_message.tx_ft = CAN_FT_DATA; /* type of frame, data or remote */
    can_transmit_message.tx_ff = CAN_FF_STANDARD; /* format of frame, standard or extended format */
    can_transmit_message.tx_dlen = 1; /* data length */
    can_transmit_message.tx_data[0] = (unsigned char) color++; /* transmit data */
    can_message_transmit(CANX, &can_transmit_message);
}

/***************************************************************************************************************/

/*!
    \brief      this function handles CANX RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
#ifdef CAN0_USED
void CAN0_RX0_IRQHandler(void)
#else
void CAN1_RX0_IRQHandler(void)
#endif
{
    CANX_RX0_IRQHandler_count++;
    rvl_led_indicator(RVL_LED_INDICATOR_LINK_CAN, TRUE);

    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &can_fifo0_receive_message);

    /* get the received message */
    can_message_receive(CANX, CAN_FIFO0, &can_fifo0_receive_message);

    can_message_transmit_color();
}

/*!
    \brief      this function handles CAN0 RX1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/

#ifdef CAN0_USED
void CAN0_RX1_IRQHandler(void)
#else
void CAN1_RX1_IRQHandler(void)
#endif
{
    CANX_RX1_IRQHandler_count++;
    rvl_led_indicator(RVL_LED_INDICATOR_LINK_CAN, TRUE);

    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &can_fifo1_receive_message);

    /* get the received message */
    can_message_receive(CANX, CAN_FIFO1, &can_fifo1_receive_message);

    can_message_transmit_color();
}

/*!
    \brief      this function handles CAN0 TX exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
#ifdef CAN0_USED
void CAN0_TX_IRQHandler(void)
#else
void CAN1_TX_IRQHandler(void)
#endif
{
    CANX_TX_IRQHandler_count++;
    rvl_led_indicator(RVL_LED_INDICATOR_LINK_CAN, TRUE);
}

/* 
 * File:   board-config.h
 * Author: morpheby
 *
 * Created on 15 June 2015, 15.16
 */

#ifndef BOARD_CONFIG_H
#define	BOARD_CONFIG_H


/* 1 if board has an oscillator, 0 otherwise */
#define HAS_EXTERNAL_OSCILLATOR 0

/* Externall oscillator frequency, if present */
//#define PRIMARY_CRYSTAL_FREQ 8000000L


/* Errata */

#define FIX_830_RAM_BUG

/* Pinmap */

// Discrete inputs
#define DI0_PIN_TYPE A
#define DI0_PIN_NUM  3
#define DI1_PIN_TYPE A
#define DI1_PIN_NUM  2
#define DI2_PIN_TYPE C
#define DI2_PIN_NUM  2
#define DI3_PIN_TYPE C
#define DI3_PIN_NUM  1

// Discrete outputs
#define VT0_PIN_TYPE B
#define VT0_PIN_NUM  15
#define VT1_PIN_TYPE B
#define VT1_PIN_NUM  14
#define VT2_PIN_TYPE C
#define VT2_PIN_NUM  7
#define VT3_PIN_TYPE C
#define VT3_PIN_NUM  6

// Devices
#define MOTOR       VT0
#define COIL        VT2
#define REVERSE     VT1
#define DETECTOR    DI3

#define __DEV_PIN_TYPE(dev) dev##_PIN_TYPE
#define DEV_PIN_TYPE(dev) __DEV_PIN_TYPE(dev)
#define __DEV_PIN_NUM(dev) dev##_PIN_NUM
#define DEV_PIN_NUM(dev) __DEV_PIN_NUM(dev)

#define HAS_LED 0
#define LED_PIN_TYPE
#define LED_PIN_NUM

#define UART_HAS_CTS    0
#define UART_HAS_RTS    0

#define UART_RX_PIN     21

//#define UART_CTS_PIN    9
//#define UART_CTS_PIN_T  B
//#define UART_CTS_LATCH  PIN_LATCH(UART_CTS_PIN_T, UART_CTS_PIN)

#define UART_TX_PIN     20
#define UART_TX_PIN2    10 // == UART_TX_PIN/2

//#define UART_RTS_PIN    23
//#define UART_RTS_PIN2   11 // == UART_RTS_PIN/2

#define CNI_0_ENABLE      0
#define CNI_1_ENABLE      0
#define CNI_2_ENABLE      0
#define CNI_3_ENABLE      0
#define CNI_4_ENABLE      0
#define CNI_5_ENABLE      0
#define CNI_6_ENABLE      0
#define CNI_7_ENABLE      0
#define CNI_8_ENABLE      0
#define CNI_9_ENABLE      1
#define CNI_10_ENABLE     0
#define CNI_11_ENABLE     0
#define CNI_12_ENABLE     0
#define CNI_13_ENABLE     0
#define CNI_14_ENABLE     0
#define CNI_15_ENABLE     0
#define CNI_16_ENABLE     0
#define CNI_17_ENABLE     0
#define CNI_18_ENABLE     0
#define CNI_19_ENABLE     0
#define CNI_20_ENABLE     0
#define CNI_21_ENABLE     0
#define CNI_22_ENABLE     0
#define CNI_23_ENABLE     0
#define CNI_24_ENABLE     0
#define CNI_25_ENABLE     0
#define CNI_26_ENABLE     0
#define CNI_27_ENABLE     0
#define CNI_28_ENABLE     0
#define CNI_29_ENABLE     0
#define CNI_30_ENABLE     0

#define CNI_9_NEEDSPU     0

#define CNI_DETECTOR    9

#define CNI_BTN_ENTER   0
#define CNI_BTN_LEFT    0
#define CNI_BTN_RIGHT   0
#define CNI_BTN_RETURN  0

#endif	/* BOARD_CONFIG_H */


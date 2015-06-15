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

#define DI0_PIN_TYPE A
#define DI0_PIN_NUM  3
#define DI1_PIN_TYPE A
#define DI1_PIN_NUM  2
#define DI2_PIN_TYPE C
#define DI2_PIN_NUM  2
#define DI3_PIN_TYPE C
#define DI3_PIN_NUM  1


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

#define CNI_1_PIN_NUM            DI3_PIN_NUM
#define CNI_1_PIN_TYPE           DI3_PIN_TYPE
#define CNI_1                    9
#define CNI_1_16                 1
#define CNI_1_NEEDSPU            0

#endif	/* BOARD_CONFIG_H */


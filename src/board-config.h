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

/* External oscillator frequency, if present */
#define PRIMARY_CRYSTAL_FREQ 8000000L


/* Errata */

// Allocates a single unused var at 0x830 RAM address
// On some devices it had fixed non-overwritable content
#define FIX_830_RAM_BUG

/* 
 * Pinmap
 * Pin type and num are obtained from processor datasheet as follows:
 * if pin is marked as RA7, TYPE is A, NUM is 7. This counts for
 * RAx, RBx, RCx. RPx are not controlled through this.
 */

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

// Onboard status LED
#define HAS_LED 0
#define LED_PIN_TYPE
#define LED_PIN_NUM

/*
 * UART pins and configuration
 * Use pin numbers from datasheet as follows:
 * if a pin is marked as RPx, x is the pin number. Pins that
 * are not marked as RPx are not supported
 */
// 1 if clear-to-send pin is used, 0 otherwise
#define UART_HAS_CTS    0
// 1 if request-to-send pin is used, 0 otherwise
#define UART_HAS_RTS    1

// RX pin number
#define UART_RX_PIN     21

// CTS pin manual control (note that RTS/CTS function wasn't tested thorougly,
// so it is highly probable its use will require some changes to the uart_base
// module. CTS latch is intended if Flow-control is not used as per nornal
// UART operation)
// NOTE: here and only here pin is used as in RAx, RBx, RCx pins, not as in RPx
#define UART_CTS_PIN    3
#define UART_CTS_PIN_T  C
#define UART_CTS_LATCH  PIN_LATCH(UART_CTS_PIN_T, UART_CTS_PIN)

// TX pin number
#define UART_TX_PIN     20
// TX pin number divided by two, floored
#define UART_TX_PIN2    10

// RTS pin number
#define UART_RTS_PIN    19
// RTS pin number divided by two, floored
#define UART_RTS_PIN2   9
#define UART_RTS_PIN_NUM 3
#define UART_RTS_PIN_TYPE C

//QEI pin numbers. Digital inputs DI3 (RP17) and DI2(RP18) are available. Data sheet page 5.
#define QEIA 17
#define QEIB 18

/*
 * Change notification pins
 * Set ENABLE to 1 to enable interrupts on the pin. Numbering is the same, as in
 * the datasheet.
 * NOTE: each enabled CN pin has to be accompanied by a CNI_x_NEEDSPU macro
 * definition, set to 1 or 0, depending whether selected input requires in-chip
 * pull-up resister or not.
 * NOTE: enabling CN pin automatically requires for a definition of accompanying
 * function in app_main (see CNI_DECL_PROC_FN)
 */
#define CNI_0_ENABLE      0
#define CNI_1_ENABLE      0
#define CNI_2_ENABLE      0
#define CNI_3_ENABLE      0
#define CNI_4_ENABLE      0
#define CNI_5_ENABLE      0
#define CNI_6_ENABLE      0
#define CNI_7_ENABLE      0
#define CNI_8_ENABLE      0
#define CNI_9_ENABLE      0
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


/*
 * LED display configuration
 */
// Define to 1 if the display is present, to 0 otherwise
// Note, that display module is quite heavy, so excluding it will save
// a lot of memory
#define HAS_LED_DISPLAY      1
// Total character count in LED display module
#define DISPLAY_COUNT        12
// Number characters in a single section of LED display module
#define CHARS_IN_SECTION     4


#define __DEV_PIN_TYPE(dev) dev##_PIN_TYPE
#define DEV_PIN_TYPE(dev) __DEV_PIN_TYPE(dev)
#define __DEV_PIN_NUM(dev) dev##_PIN_NUM
#define DEV_PIN_NUM(dev) __DEV_PIN_NUM(dev)

/*
 * Devices
 * Put here ids of onboard devices for easier comprehension of code. Use as index
 * for discrete_* functions
 */

// CN-attached devices
// Use CNx pin number, to associate with the CNI_DECL_PROC_FN macro
#define CNI_DETECTOR    29

// These four are used in the menu module. The module will not compile, if this
// configuration is not set
#define CNI_BTN_ENTER   0
#define CNI_BTN_LEFT    0
#define CNI_BTN_RIGHT   0
#define CNI_BTN_RETURN  0

//ADC channels configuration
#define ADC_0_ENABLE 1
#define ADC_1_ENABLE 1
#define ADC_2_ENABLE 1
#define ADC_3_ENABLE 0
#define ADC_4_ENABLE 0
#define ADC_5_ENABLE 0
#define ADC_6_ENABLE 0

#define ADC_CHANNEL_COUNT ADC_0_ENABLE + ADC_1_ENABLE + ADC_2_ENABLE    \
                         + ADC_3_ENABLE + ADC_4_ENABLE + ADC_5_ENABLE   \
                         + ADC_6_ENABLE

#endif	/* BOARD_CONFIG_H */


/* 
 * File:   app_main.h
 * Author: morpheby
 *
 * Created on пятніца, 19, чрвеня 2015, 11.14
 */

#ifndef APP_MAIN_H
#define	APP_MAIN_H

/*
 * Module configuration
 */

// Use ADC module
#define APP_USE_ADC             1

// Use CN module
#define APP_USE_CN              1

// Use interactive menu
#define APP_USE_MENU            0

// Use modbus dynamic addressing extensions
#define APP_USE_MODBUS_EXT      1

// Use Real-time self-programming module
// Note that disabling RTSP module here will not free one-page section .fixed
// in linker script, so if you require some memory, go to the linker script and
// remove the section after disabling the module
#define APP_USE_RTSP            1

// Use LED display extensions (segmentation, rounding, scrolling, string display)
#define APP_USE_LED_DISP        0


/*
 * UART configuration
 */

// Requests using 9-Bit mode + addressing
#define UART_USE_9BIT_MODE  0

// UART baudrate
#define UART_BAUDRATE       19200


/* 
 * Event-based functions
 */

// Main loop addition. See MAIN_DECL_LOOP_FN
#define MAIN_LOOP_FN_ENABLE     1

// Custom processing of discrete outputs. See DIO_DECL_UPDATE_FN
#define DIO_UPDATE_FN_ENABLE    1


/*
 * Internal function declarations
 */
void app_init();

#endif	/* APP_MAIN_H */



#include "uart_base.h"
#include <uart.h>
#include <stdlib.h>
#include "ipl-config.h"
#include "board-config.h"

#define UART_BRGVAL         (UART_RATE_CYCLES/16 - 1)

void uart_init() {
    
    /* Initialize UART1 */
    U1BRG = UART_BRGVAL;

    /* Configure pins */
    pin_remap_enable(1);
    // Input configuration
    RPINR18bits.U1RXR   = UART_RX_PIN;
#if UART_HAS_CTS
    RPINR18bits.U1CTSR  = UART_CTS_PIN;
#endif
    // Output configuration
    RPOR_MAP(UART_TX_PIN, UART_TX_PIN2) = 0b00011;
#if UART_HAS_RTS
    RPOR_MAP(UART_RTS_PIN, UART_RTS_PIN2) = 0b00100;
//    ODC_BIT(UART_RTS_PIN_TYPE, UART_RTS_PIN_NUM) = 1;
#endif
    
    pin_remap_enable(0);

    /* Configure UART params */
    U1MODEbits.STSEL = 0;  // One stop-bit
#if UART_USE_9BIT_MODE
    U1MODEbits.PDSEL = 0b11; // 9-bit
#else
    U1MODEbits.PDSEL = 0b00; // 8-bit, no parity
#endif
    U1MODEbits.ABAUD = 0;
    U1MODEbits.BRGH = 0;
#if UART_HAS_CTS && UART_HAS_RTS
    U1MODEbits.UEN = 0b10; // U1RX, U1TX, U1CTS and U1RTS are enabled
#elif UART_HAS_CTS
    U1MODEbits.UEN = 0b11; // U1RX, U1TX and U1CTS are enabled
#elif UART_HAS_RTS && !UART_MANUAL_RTS
    U1MODEbits.UEN = 0b01; // U1RX, U1TX and U1RTS are enabled
#else
    U1MODEbits.UEN = 0b00; // Only U1RX and U1TX are enabled
#endif
    U1MODEbits.URXINV = 0; // XXX This value depends on board configuration
#if UART_HAS_RTS
    U1MODEbits.RTSMD = 1; // U1RTS: 0 - Flow control, 1 - Simplex mode
#endif
    U1STAbits.UTXINV = 0; // XXX This value depends on board configuration
//    UART_CTS_LATCH = 0; // XXX Without this send never starts. Maybe

#if UART_USE_9BIT_MODE
    U1STAbits.ADDEN = 1; // Enable 9th bit block
#endif

    /* TX interrupt settings */
    U1STAbits.UTXISEL0 = 0; // Set interrupt when there is free space
    U1STAbits.UTXISEL1 = 0; // in UART internal buffer.
    IPC3bits.U1TXIP = IPL_UART_TX;
    IEC0bits.U1TXIE = 1;    // Enable UART TX interrupt

    /* RX interrupt settings */
    U1STAbits.URXISEL0 = 0; // interrupt on one char received
    U1STAbits.URXISEL1 = 0;
    IPC2bits.U1RXIP = IPL_UART_RX;
    IEC0bits.U1RXIE = 1;    // Enable UART RX interrupt

    IPC16bits.U1EIP = IPL_UART_ERR;

    /* Start UART */
    U1MODEbits.UARTEN = 1;  // Start UART
    U1STAbits.UTXEN   = 1;  // Start UART TX

    // lock system for one UART cycle
    system_lock(UART_RATE_CYCLES);
}
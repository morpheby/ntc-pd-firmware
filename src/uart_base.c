
#include "uart_base.h"
#include <uart.h>
#include <stdlib.h>
#include "ipl-config.h"
#include "board-config.h"

#define UART_BRGVAL         (UART_RATE_CYCLES/16 - 1)

/* Maintain UART status between resets */

/**** UART RX buffers and parameters ****/
_PERSISTENT unsigned char *uartBufferR, *uartBufferW,
        *uartBufferStart, *uartBufferEnd;
_PERSISTENT unsigned int uartBufLen;

#if UART_USE_9BIT_MODE
_PERSISTENT _Bool isUartRecieveActive;
#else
#define isUartRecieveActive 1
#endif

_PERSISTENT uint8_t deviceID;

_PERSISTENT _Bool uartRXLock;

/**** UART TX buffers and parameters ****/
#if UART_USE_9BIT_MODE
_PERSISTENT uint16_t
#else
_PERSISTENT uint8_t
#endif
        *uartTransmitData,
        *uartTransmitPos;
_PERSISTENT unsigned int uartTransmitLen, uartTransmitBufferSize;
_Bool uartTXLock;
_Bool uartSlowModeOn = 0;

_Bool uartSyncSet = 0;
_Bool uartSyncLock = 0;

// 4 times larger than internal UART's one
#define UART_MIN_BUFF_SIZE  16

void uart_set_addr(uint8_t addr) {
    deviceID = addr;
}

uint8_t uart_get_addr() {
    return deviceID;
}

_Bool uart_is_my_addr(uint8_t addr) {
    return addr == UART_GLOBAL_ADDR ||
           addr == deviceID;
}

void uart_rx_lock() {
    IEC0bits.U1RXIE = 0;
    uartRXLock      = 1;
}

void uart_tx_lock() {
    IEC0bits.U1TXIE = 0;
    uartTXLock      = 1;
}

void uart_rx_unlock() {
    IEC0bits.U1RXIE = 1;
    uartRXLock    = 0;
}

void uart_tx_unlock() {
    IEC0bits.U1TXIE = 1;
    uartTXLock    = 0;
}

void uart_garbage_collect() {
    // Something needs memory, shrink UART recieve buffer as much,
    // as we can
    size_t newSize;

    // Since we are working with UART data, lock UART
    // interrupts
    uart_rx_lock();
        newSize = uartBufLen + 1 < UART_MIN_BUFF_SIZE ?
            UART_MIN_BUFF_SIZE : uartBufLen + 1;

        if(uartBufLen) {
            unsigned char *tmpBuff;
            unsigned int i;
            tmpBuff = gc_malloct(newSize);

            // Copy data to temp buffer
            for(i = 0; uartBufferR != uartBufferW; ++uartBufferR, ++i)
                tmpBuff[i] = *uartBufferR;

            gc_free(uartBufferStart);
            uartBufferStart = gc_malloct(newSize);
            /* # - allocated mem; @ - other mem
             *     uartBufferStart       uartBufferEnd
             *     v       <-- newSize -->           v
             * @ @ # # # # # # # # # # # # # # # # # @ @ @ @
             *     ^     <-- uartBufLen -->        ^
             *     uartBufferR             uartBufferW
             * NOTE: initially we set write pointer to start
             *       position. Later, when we copy buffer back
             *       it will eventually move to the correct position.
             */
            uartBufferEnd = newSize +
                    (uartBufferW = uartBufferR = uartBufferStart);

            // Copy data to the new buffer
            for(i = 0; i<uartBufLen; ++i)
                *uartBufferW++ = tmpBuff[i];

            gc_free(tmpBuff);
        }
    uart_rx_unlock();
    
    uart_tx_lock();
        if (uartTransmitLen == 0 && uartTransmitData) {
            gc_free(uartTransmitData);
            uartTransmitBufferSize = 0;
            uartTransmitData = 0;
            uartTransmitPos = 0;
        }
    uart_tx_unlock();
}

void uart_start_recieve() {
#if UART_USE_9BIT_MODE
    isUartRecieveActive = 1;
    U1STAbits.ADDEN = 0;
#endif
}

void uart_stop_recieve() {
#if UART_USE_9BIT_MODE
    isUartRecieveActive = 0;
    U1STAbits.ADDEN = 1;
#endif
}

void _uart_read_byte() {
    if(! U1STAbits.URXDA)
        return;

    uint16_t recievedWord = U1RXREG;

#if UART_USE_9BIT_MODE
    if(!isUartRecieveActive) {
        /* Check target */
        if(uart_is_my_addr(recievedWord))
            uart_start_recieve();
        return;
    }

    // if recieved word is address, and recieving is enabled,
    // check if that's us who is addressed and if not, stop
    // recieving
    if(recievedWord & (1 << 8)) {
        if(!uart_is_my_addr(recievedWord))
            uart_stop_recieve();
        return;
    }
#endif

    /* Read byte */
    *(uartBufferW++) = recievedWord;
    ++uartBufLen;

    /* Cycle buffer */
    if(uartBufferW == uartBufferEnd)
        uartBufferW = uartBufferStart;

    if(uartBufLen >= uartBufferEnd - uartBufferStart) {
        /* Expand buffer */
        // Store memory offsets
        size_t writeOffset = uartBufferW - uartBufferStart,
               readOffset  = uartBufferR - uartBufferStart,
               bufSize     = uartBufferEnd - uartBufferStart,
               endOffset   = bufSize;

        unsigned char *bufStartNew;

        // Reallocate memory
        bufSize *= 2; // double memory size
        while(!(bufStartNew = gc_realloc(uartBufferStart, bufSize)))
            --bufSize; // Seems that there isn't enough memory

        // Check if new buffer is affordable
        if(bufSize <= uartBufLen) {
            // Seems we were not able to afford new size.
            // Discard the whole buffer and stop recieving
            //uart_send(UART_GLOBAL_ADDR, UART_STATUS_BUFFAIL);
            uart_stop_recieve();
            uartBufLen = 0;
            gc_free(bufStartNew);
            uartBufferStart = gc_malloct(UART_MIN_BUFF_SIZE);
            // Reset pointers
            uartBufferEnd = UART_MIN_BUFF_SIZE +
                    (uartBufferR = uartBufferW = uartBufferStart);
            return;
        }

        // Restore vars
        uartBufferStart = bufStartNew;
        uartBufferEnd   = uartBufferStart + bufSize;
        uartBufferW     = uartBufferStart + writeOffset;
        uartBufferR     = uartBufferStart + readOffset;

        // Correct cycle ring
        if(uartBufferR >= uartBufferW) {
            unsigned char *newTail = uartBufferEnd,
                          *oldTail = uartBufferStart + bufSize;
            for(; oldTail > uartBufferStart + readOffset; --newTail, --oldTail) {
                *newTail = *oldTail;
            }
            uartBufferR = newTail;
        }

    }
}

void uart_write_byte() {
    if(!uartTransmitLen){
        // No data. Ignore
        UART_CTS_LATCH = 0;
        return;
    }
    UART_CTS_LATCH = 1;
    while(uartTransmitPos < uartTransmitData + uartTransmitLen &&
            !U1STAbits.UTXBF) {
        // Fill in transmit buffer
        U1TXREG = *(uartTransmitPos++);
        if(uartSlowModeOn) {
            while(!uart_tx_empty());
            int i;
            for(i = 0; i < UART_RATE_CYCLES; ++i)
                Nop();
        }
    }
    if(uartTransmitPos == uartTransmitData + uartTransmitLen) {
        // All data was successfully transferred
        // Free the buffer
        uart_tx_lock();
            uartTransmitPos = uartTransmitData;
            uartTransmitLen = 0;
        uart_tx_unlock();
    }    
}

int uart_get_data_size() {
    return uartBufLen;
}

_Bool uart_is_data_ready() {
    return uart_get_data_size() || U1STAbits.URXDA;
}

void uart_flush_rx() {
    if(U1STAbits.RIDLE)
        while(U1STAbits.URXDA)
            _uart_read_byte();

    uart_rx_lock();
        if(U1STAbits.OERR)
            U1STAbits.OERR = 0;

        uartBufLen = 0;
        uartBufferR = uartBufferW = uartBufferStart;
    uart_rx_unlock();
}

size_t uart_recieve_nolock(unsigned char *data, size_t bufLen) {
    unsigned int i;

    /* Check for hanging data in buffer */
    if(U1STAbits.RIDLE)
        while(U1STAbits.URXDA)
            _uart_read_byte();

    /* Check for buffer overflow */
    if(U1STAbits.OERR)
        // Reset buffer
        U1STAbits.OERR = 0;

    /* Finally, read stored data ;) */
    for(i = 0; i < bufLen && uartBufferR != uartBufferW;
            ++i, ++data, ++uartBufferR, --uartBufLen) {
        if (uartBufferR == uartBufferEnd) {
            uartBufferR = uartBufferStart;
        }
        *data = *uartBufferR;
    }
    
    return i;
}

void uart_recieve(unsigned char *data, size_t len) {
    while(len) {
        size_t recieved = uart_recieve_nolock(data, len);
        len  -= recieved;
        data += recieved;
    }
}

void uart_send(uint8_t addr, const unsigned char *data, size_t len) {
    size_t oldDataLen = uartTransmitLen;
    ptrdiff_t dataPosOffset = uartTransmitPos - uartTransmitData;
    uint8_t *uartTransmitDataNew;
    size_t uartTransmitBufferSizeNew;

    // Expand (or allocate) buffer to hold new data
    uart_tx_lock();
        uartTransmitLen += len;
#if UART_USE_9BIT_MODE
        uartTransmitLen += 3;
#endif
        if (uartTransmitBufferSize < uartTransmitLen) {
            do {
                uartTransmitBufferSizeNew = uartTransmitLen;
                uartTransmitDataNew = gc_realloc(uartTransmitData,
#if UART_USE_9BIT_MODE
                    uartTransmitBufferSizeNew*sizeof(uint16_t));
#else
                    uartTransmitBufferSizeNew*sizeof(uint8_t));
#endif
                if (!uartTransmitDataNew) {
                    if (oldDataLen != 0) {
                        uart_tx_unlock();
                        while(!uart_tx_empty()) {}
                        uart_tx_lock();
                    } else {
                        system_fail(_StrNoMemory);
                    }
                }
            } while (!uartTransmitDataNew);
            
            uartTransmitData = uartTransmitDataNew;
            uartTransmitBufferSize = uartTransmitBufferSizeNew;
            
            uartTransmitPos = uartTransmitData + dataPosOffset;
        }
        
#if UART_USE_9BIT_MODE
        // Put address (twice)
        uartTransmitData[oldDataLen++] = (1 << 8) | addr;
        uartTransmitData[oldDataLen++] = (1 << 8) | addr;
#endif
        
        // Put data
        for(; len; ++oldDataLen, ++data, --len) {
            uartTransmitData[oldDataLen] = *data;
        }
#if UART_USE_9BIT_MODE
        uartTransmitData[oldDataLen++] = (1 << 8) | UART_NO_ADDR; // EOF
#endif
    uart_tx_unlock();

    if(U1STAbits.TRMT) {
        // Kick-start transfer
        ++uartTransmitPos;
        U1TXREG = *(uartTransmitPos-1);
    }
}

_Bool uart_tx_empty() {
    return U1STAbits.TRMT;
}

void uart_slow_mode(_Bool on) {
    uartSlowModeOn = on;
}

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

    /* Initialize variables */

    /*** RX config ***/
    if(!reset_is_cold() && uartRXLock)
        // Seems we are in trouble. Try freeing buffers and
        // reinitialize them
        gc_free(uartBufferStart);
    // UART buffers are not affected by SW resets
    if(reset_is_cold() || uartRXLock) {
        uart_set_addr(UART_NO_ADDR);

        // allocate buffer
        uartBufferStart = gc_malloct(UART_MIN_BUFF_SIZE);
        uartBufferEnd = UART_MIN_BUFF_SIZE +
                (uartBufferR = uartBufferW = uartBufferStart);
        uartBufLen = 0;

#if UART_USE_9BIT_MODE
        // Initially we are not recieveing data
        isUartRecieveActive = 0;
#endif
        // UART is not locked
        uartRXLock = 0;

        // Register garbage collector
        garbage_collect_reg(uart_garbage_collect);
    }

    /*** TX config ***/
    if(!reset_is_cold() && uartTXLock)
        // Seems we are in trouble. Try freeing buffers and
        // reinitialize them
        gc_free(uartTransmitData);
    if(reset_is_cold() || uartTXLock) {
        uartTransmitData = uartTransmitPos = 0;
        uartTransmitLen = uartTransmitBufferSize = 0;

        uartTXLock = 0;
    }

    /* Start UART */
    U1MODEbits.UARTEN = 1;  // Start UART
    U1STAbits.UTXEN   = 1;  // Start UART TX

    // lock system for one UART cycle
    system_lock(UART_RATE_CYCLES);
}

void uart_sync_recieve(_SyncFunction synchronize) {
    uint8_t buff;

    /* Prepare line */
    uartSyncLock = 1; // Stop normal operation
    while(U1STAbits.URXDA)
        buff = U1RXREG; // discard everything left in buffer
    U1STAbits.ADDEN = 0; // Recieve everything


    /* Configure baudrate */
    // Recieve break char to ensure line is clear
    U1MODEbits.WAKE = 1;
    U1MODEbits.ABAUD = 1;

    while(U1MODEbits.ABAUD);

    /* Reset timer */
    U1MODEbits.WAKE = 1;
    while(U1MODEbits.WAKE);

    synchronize();
    
    U1STAbits.ADDEN = 1;
    uartSyncLock = 0;
}

void uart_sync_transmit(_SyncFunction synch) {
    /* Prepare line */
    while(!U1STAbits.TRMT) // Wait until all transfer operations complete
    uartSyncLock = 1; // Stop normal operation

    // Transfer Break char followed by Sync char
    U1STAbits.UTXBRK = 1;
    U1TXREG = 0x00; // transimt break
    U1TXREG = 0x55; // transimt sync

    while(!U1STAbits.TRMT);

    U1STAbits.UTXBRK = 1;
    U1TXREG = 0x00; // transimt break
    while(U1STAbits.UTXBRK);

    synch();

    uartSyncLock = 0;
}

void _ISR_NOPSV _U1ErrInterrupt() {
    led_flash(FCY); // XXX Don't think that may be needed in production
    led_flash(FCY);
    led_flash(FCY);
    IFS4bits.U1EIF = 0; // ignore
}

/* UART TX Interrupt */
void _ISR_NOPSV _U1TXInterrupt() {
    if(!uartSyncLock) {
        uart_write_byte();
    }
    IFS0bits.U1TXIF = 0;
}

/* UART RX Interrupt */
void _ISR_NOPSV _U1RXInterrupt() {
    if(!uartSyncLock)
        _uart_read_byte();

    // Clear interrupt flag
    IFS0bits.U1RXIF = 0;
}

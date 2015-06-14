/* 
 * File:   uart_base.h
 * Author: Morphe
 *
 * Created on 2 June 2012, 11:02
 */

#ifndef UART_BASE_H
#define	UART_BASE_H

#include "system.h"

/* Requests using 9-Bit mode + addressing */
#define UART_USE_9BIT_MODE  0

#define UART_GLOBAL_ADDR    0xFF
#define UART_NO_ADDR        0x00

#define UART_IPL 6

#define UART_BAUDRATE       19200
#define UART_RATE_CYCLES    (FCY/UART_BAUDRATE)

void uart_init();

_Bool uart_is_data_ready();
int uart_get_data_size();

_Bool uart_tx_empty();

size_t uart_recieve_nolock(unsigned char *data, size_t bufLen);
void uart_recieve(unsigned char *data, size_t len);

void uart_flush_rx();

void uart_send(uint8_t addr, const unsigned char *data, size_t len);

void uart_set_addr(uint8_t addr);
uint8_t uart_get_addr();

typedef void(*_SyncFunction)();
void uart_sync_transmit(_SyncFunction synchronize);
void uart_sync_recieve(_SyncFunction synchronize);

void uart_slow_mode(_Bool on);

#endif	/* UART_BASE_H */


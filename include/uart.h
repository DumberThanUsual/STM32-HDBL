#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

#define NUCLEO

#ifdef NUCLEO
#define COMPORT USART2
#else
#define COMPORT USART1
#endif

typedef enum 
{
    U_OK,
    U_ERROR,
    U_TIMEOUT
} uart_ret_t;

void uart_init();
void uart_deinit();
void uart_tx_string(uint8_t *);
void uart_tx_byte(uint8_t);

uart_ret_t uart_rx_byte(uint8_t *byte, uint32_t timeout);
uart_ret_t uart_rx_string(uint8_t *string, uint32_t length, uint32_t timeout);
uart_ret_t uart_rx_wait_available(uint32_t timeout);

#endif
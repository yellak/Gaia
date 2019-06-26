#ifndef _UART_LIB_
#define _UART_LIB_

#include <msp430.h>
#include <stdint.h>

void uartInit(uint32_t br, uint8_t isMSB, uint8_t par, uint8_t nStops);

void uartSend(uint8_t byte);

int uartGet(uint8_t *data);

int uartSendStr(uint8_t string[]);

#endif

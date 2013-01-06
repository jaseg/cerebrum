#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init();
uint16_t uart_getc(void);
void uart_putc(uint8_t data);

#endif // UART_H 


#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);
void uart_putc(uint8_t data);
void uart_putc_nonblocking(uint8_t data);

#endif // UART_H 


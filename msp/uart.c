/*
 * This file is part of the MSP430 hardware UART example.
 *
 * Copyright (C) 2012 Stefan Wendler <sw@kaltpost.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
 * Hardware UART example for MSP430.
 *
 * Stefan Wendler
 * sw@kaltpost.de
 * http://gpio.kaltpost.de
 ******************************************************************************/

#include <msp430.h>
#include <legacymsp430.h>

#include "comm.h"
#include "comm_handle.h"
#include "uart_msp.h"

unsigned char uart_txfifo[32];
unsigned char *uart_txfifo_head = uart_txfifo;
unsigned char *uart_txfifo_tail = uart_txfifo;

void uart_init(void)
{
  	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  	UCA0BR0 = 138;                            // 16MHz 115200
  	UCA0BR1 = 0;                              // 16MHz 115200
  	UCA0BR1 = 0;                              // 16MHz 115200
  	UCA0MCTL = UCBRS2;                        // Modulation UCBRSx = 1
  	UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
  	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

int uart_getc()
{
    if(!(IFG2&UCA0RXIFG))
		return -1;
	return UCA0RXBUF;
}

void uart_loop(){
	if(uart_txfifo_head != uart_txfifo_tail){
		if(!uart_try_putc(*uart_txfifo_tail)){
			uart_txfifo_tail++;
			if(uart_txfifo_tail > uart_txfifo+sizeof(uart_txfifo))
				uart_txfifo_tail = uart_txfifo;
		}
	}
}

void uart_putc_nonblocking(unsigned char c){
	*uart_txfifo_head = c;
	uart_txfifo_head++;
	if(uart_txfifo_head > uart_txfifo+sizeof(uart_txfifo))
		uart_txfifo_head = uart_txfifo;
}

unsigned char uart_try_putc(unsigned char c){
	if(!(IFG2&UCA0TXIFG))
		return 1;
  	UCA0TXBUF = c;                    		// TX
	return 0;
}

void uart_putc(unsigned char c)
{
	while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
  	UCA0TXBUF = c;                    		// TX
}

interrupt(USCIAB0RX_VECTOR) USCI0RX_ISR(void)
{
	comm_handle(UCA0RXBUF);
}

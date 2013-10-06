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
 *
 *****************************************************************************/

#ifndef __UART_H
#define __UART_H

void uart_init(void);

void uart_loop(void);

/* Read a character or return -1 if none is available */
int uart_getc(void);

/* Write one chracter to the UART blocking */
void uart_putc(unsigned char c);

/* Try to put a character. Returns 0 on success, 1 if it failed */
unsigned char uart_try_putc(unsigned char c);

extern unsigned char uart_txfifo[32];
extern unsigned char *uart_txfifo_head;
extern unsigned char *uart_txfifo_tail;

#endif


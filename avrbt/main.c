/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <comm.h>
#include <autocode.h>
#include <config.h>
#include <uart.h>

/* XXX FIXME TEST CODE!!1! */
#define BTNAME "CerebrumTest"

void init(void);

int main(void){
    init();
    for(;;) loop_auto();
}

void init(){

    wdt_disable();
    uart_init(UART_BAUD_SELECT(38400, F_CPU));

	/* enable bluetooth module */
	DDRC |= 2;
	PORTC &= ~2;

	_delay_ms(200);
	/* Init bluetooth modem as slave */
	uart_puts_p(PSTR("\r\n+STWMOD=0\r\n"));			/* enter slave mode */
	_delay_ms(100);
	uart_puts_p(PSTR("\r\n+STNA=" BTNAME "\r\n"));	/* set the bluetooth name */
	_delay_ms(100);
	uart_puts_p(PSTR("\r\n+STOAUT=1\r\n"));			/* permit paired device to connect */
	_delay_ms(100);
	uart_puts_p(PSTR("\r\n+STAUTO=1\r\n"));			/* enable auto-connect */
	_delay_ms(2000); /* wait for the bluetooth module */
	uart_puts_p(PSTR("\r\n+INQ=1\r\n"));			/* make the slave bluetooth inquirable  */
	_delay_ms(100);

	init_auto();

    sei();
}


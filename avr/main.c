/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "comm.h"
#include "autocode.h"

void init(void);
void loop(void);

int main(void){
    init();
    for(;;) loop();
}

void init(){
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200, F_CPU));
	init_auto();
    sei();
}

void loop(){
	/* FIXME HISTORIC DOC ONLY
#ifdef HAS_NOISE_MAKER_SUPPORT
	case 'x':
		//DDRF |= 0x01;
		//PORTF |= 0x01;
		break;
	case 'X':
		//DDRF &= 0xFE;
		//PORTF &= 0xFE;
		break;
#endif//HAS_NOISE_MAKER_SUPPORT
		*/
	comm_loop();
	loop_auto();
	//FIXME remove this delay?
    //_delay_us(100);
}

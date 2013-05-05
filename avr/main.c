/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include <comm.h>
#include <autocode.h>
#include <config.h>
#include <uart.h>

void init(void);

int main(void){
    init();
    for(;;) loop_auto();
}

void init(){

    wdt_disable();
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(CEREBRUM_BAUDRATE, F_CPU));

	init_auto();

    sei();
}


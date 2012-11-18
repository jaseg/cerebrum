/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#ifndef TEST
#include <avr/interrupt.h>
#include <avr/io.h>
#else
#define sei()
#endif//TEST
#include "comm.h"
#include "autocode.h"
#include "config.h"
#include "uart.h"

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
	comm_loop();
	loop_auto();
}

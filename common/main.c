/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#ifdef __AVR__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#endif

#include <comm.h>
#include <autocode.h>
#include <config.h>
#include <uart.h>
#ifdef __TEST__
#include "comm_handle.h"
#endif

void init(void);

int main(void){
    init();
#ifdef __TEST__
    //debug stuff
    int16_t v;
    while((v = getchar()) >= 0){
        comm_handle(v);
    }
#else
    for(;;) loop_auto();
#endif
}

void init(){

#if defined(__MSPGCC__)
    WDTCTL = WDTPW | WDTHOLD; //Disable WDT

    //oscillator control
    DCOCTL |= DCO1 | DCO0;
    BCSCTL1 |= RSEL3 | RSEL2 | RSEL0; 
    
#ifndef __MSP430G2553__
#error No port setup for that part. Please fix this!
#endif//__MSP430G2553__
    //UART io setup for the msp430g2553 (RX: P1.1, TX: P1.2)
    P1SEL  |= 0x03;
    P1SEL2 |= 0x03;
#elif defined(__AVR__)//__MSPGCC__
    wdt_disable();
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(CEREBRUM_BAUDRATE, F_CPU));
#endif//AVR

	init_auto();

    //enable interrupts
#if defined(__AVR__)
    sei();
#elif defined(__MSPGCC__)//__AVR__
    _BIS_SR(GIE);
#endif//__MSPGCC__
}


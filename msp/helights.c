/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <msp430.h>
#include "autocode.h"

int main(void){
    WDTCTL = WDTPW | WDTHOLD; //Disable WDT
    P1DIR = 0x41; //P1.6 (green led) and P1.0 (red LED) outputs

    //oscillator control
    DCOCTL |= DCO1 | DCO0;
    BCSCTL1 |= RSEL3 | RSEL2 | RSEL0; 
    
    //UART io setup (RX: P1.1, TX: P1.2)
    P1SEL  |= 0x03;
    P1SEL2 |= 0x03;

    init_auto();
    
    //set global interrupt enable bit
    _BIS_SR(GIE);

    for(;;){
        loop_auto();
        unsigned int i = 0xffff;
        while(--i);
    }
}

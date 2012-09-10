/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <msp430.h>
#include <math.h>

typedef struct{
    unsigned int r, g, b;
} rgbval; 

typedef struct{
    float h, s, v;
} hsvval;

rgbval hsv2rgb(hsvval in){
    int hi = (int)floorf(in.h/60.0F);
    float f = (in.h/60.0F-hi);
    float p = in.v*(1.0F-in.s);
    float q = in.v*(1.0F-in.s*f);
    float t = in.v*(1.0F+in.s*f-in.s);
    rgbval ret;
    switch(hi){
        case 0:
            ret.r = in.v*0xFFFF;
            ret.g = t*0xFFFF;
            ret.b = p*0xFFFF;
            break;
        case 1:
            ret.r = q*0xFFFF;
            ret.g = in.v*0xFFFF;
            ret.b = p*0xFFFF;
            break;
        case 2:
            ret.r = p*0xFFFF;
            ret.g = in.v*0xFFFF;
            ret.b = t*0xFFFF;
            break;
        case 3:
            ret.r = p*0xFFFF;
            ret.g = q*0xFFFF;
            ret.b = in.v*0xFFFF;
            break;
        case 4:
            ret.r = t*0xFFFF;
            ret.g = p*0xFFFF;
            ret.b = in.v*0xFFFF;
            break;
        case 5:
            ret.r = in.v*0xFFFF;
            ret.g = p*0xFFFF;
            ret.b = q*0xFFFF;
            break;
    }
    return ret;
}

int main(void){
    WDTCTL = WDTPW | WDTHOLD; //Disable WDT
    P1DIR = 0x41; //P1.6 (green led) and P1.0 (red LED) outputs

    //oscillator control
    DCOCTL |= DCO1 | DCO0;
    BCSCTL1 |= RSEL3 | RSEL2 | RSEL0; 
    
    //Timer setup
    TA0CTL = MC_2 | TASSEL_2; //Continous with SMCLK
    TA1CTL = MC_2 | TASSEL_2; //Continous with SMCLK
    TA0CCTL1 = OUTMOD_3;
    TA1CCTL1 = OUTMOD_3;
    TA1CCTL2 = OUTMOD_3;

    P1DIR |= 0x40; //P1.6
    P1SEL |= 0x40;
    P2DIR |= 0x12; //P2.1 2.4
    P2SEL |= 0x12;
    
    _BIS_SR(GIE);

    hsvval col = {0.0, 1.0, 1.0};
    unsigned int foo = 0;
    for(;;){
        col.h += 0.01;
        if(col.h > 360)
            col.h = 0;
        foo++;
        //P1OUT^=0x01;
        rgbval rgb = hsv2rgb(col);
        TA0CCR1 = rgb.g;
        TA1CCR1 = rgb.b;
        TA1CCR2 = rgb.r;
        unsigned int i=0x20;
        while(--i);
    }
}

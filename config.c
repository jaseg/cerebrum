
#include "config.h"
#include <avr/io.h>
#include "input.h"

void config_setup(){
}

//FIXME This is a monstrum. A hardware fix is urgently required.
void led_output_stuff1(uint8_t data, uint8_t dir){
    // Q&0x01 ==> PG5
    //     02 ==> PE3
    //     04 ==> PH3
    //     08 ==> PH4
    //     10 ==> PH5
    //     20 ==> PH6
    //     40 ==> PB4
    //     80 ==> PB5
    DDRG&=~(1<<5);
    DDRG|=(DDRQ&1)<<5;
    PORTG&=~(1<<5);
    PORTG|=(Q&1)<<5;
    DDRE&=~(1<<3);
    DDRE|=(DDRQ&2)<<2;
    PORTE&=~(1<<3);
    PORTE|=(Q&2)<<2;
    DDRH&=0xC3;
    DDRH|=(DDRQ&0x3C);
    PORTH&=0xC3;
    PORTH|=(Q&0x3C);
    DDRB&=0xCF;
    DDRB|=(DDRQ&0xC0)>>2;
    PORTB&=0xCF;
    PORTB|=(Q&0xC0)>>2;
}


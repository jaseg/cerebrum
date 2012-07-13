
#include "config.h"
#include <avr/io.h>
#include "input.h"

void config_setup(){
}

void pwm_output_setup(){
    DDRE |= 0x30;
    DDRB |= 0xC0;
}

void pwm_unset_outputs(){
    PORTE &= ~0x30;
    PORTB &= ~0xC0;
}

void pwm_set_outputs(uint8_t data){
    data <<= 4;
    PORTE |= data&0x30;
    PORTB |= data&0xC0;
}

//FIXME This is a monstrum. A hardware fix is urgently required.
void led_output_stuff1(uint8_t data, uint8_t dir){
    // data&0x01 ==> PG5
    //     02 ==> PE3
    //     04 ==> PH3
    //     08 ==> PH4
    //     10 ==> PH5
    //     20 ==> PH6
    //     40 ==> PB4
    //     80 ==> PB5
    DDRG&=~(1<<5);
    DDRG|=(dir&1)<<5;
    PORTG&=~(1<<5);
    PORTG|=(data&1)<<5;
    DDRE&=~(1<<3);
    DDRE|=(dir&2)<<2;
    PORTE&=~(1<<3);
    PORTE|=(data&2)<<2;
    DDRH&=0xC3;
    DDRH|=(dir&0x3C);
    PORTH&=0xC3;
    PORTH|=(data&0x3C);
    DDRB&=0xCF;
    DDRB|=(dir&0xC0)>>2;
    PORTB&=0xCF;
    PORTB|=(data&0xC0)>>2;
}


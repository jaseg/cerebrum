#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "7seg_config.h"

void 7seg_setup(){
    7SEG_CLK_DDR |= _BV(7SEG_CLK_PIN);
    7SEG_DATA_DDR |= _BV(7SEG_DATA_PIN);
    7SEG_DIGIT1_DDR |= _BV(7SEG_DIGIT1_PIN);
    7SEG_DIGIT2_DDR |= _BV(7SEG_DIGIT2_PIN);
}

inline void 7seg_pulse_clk(){
    7SEG_CLK_PORT |= _BV(7SEG_CLK_PIN);
    7SEG_CLK_PORT &= ~_BV(7SEG_CLK_PIN);
}

inline void 7seg_data_out(uint8_t val){
    7SEG_DATA_PORT &= ~_BV(7SEG_DATA_PIN);
    if(!val){
        7SEG_DATA_PORT |= _BV(7SEG_DATA_PIN);
    }
}

inline void 7seg_select_digit(uint8_t digit){
    7SEG_DIGIT1_PORT &= ~_BV(7SEG_DIGIT1_PIN);
    7SEG_DIGIT2_PORT &= ~_BV(7SEG_DIGIT2_PIN);
    if(digit&1){ 
        7SEG_DIGIT1_PORT |= _BV(7SEG_DIGIT1_PIN);
    }
    if(digit&2){
        7SEG_DIGIT2_PORT |= _BV(7SEG_DIGIT2_PIN);
    }
}

int 7seg_cycle=1;

int 7seg_digit1[] = {
    0x56C0,
    0x6000,
    0x94C0,
    0x9640,
    0xC600,
    0xD240,
    0xD2C0,
    0x1600,
    0xD6C0,
    0xD640,
    0xD680,
    0xC2C0,
    0x50C0,
    0x86C0,
    0xD0C0,
    0xD080
};

int 7seg_digit2[] = {
    0x011F,
    0x000C,
    0x081B,
    0x081E,
    0x090C,
    0x0916,
    0x0917,
    0x001C,
    0x091F,
    0x091E,
    0x091D,
    0x0907,
    0x0113,
    0x080F,
    0x0913,
    0x0911
};

int 7seg_get_digit(uint8_t b, uint8_t a){
    int out = 0;
    a -= '0';
    b -= '0';
    if(a > 0x20)
        a-=0x20;
    if(a > 0x10)
        a-=7;
    if(b > 0x20)
        b-=0x20;
    if(b > 0x10)
        b-=7;
    out = 7seg_digit1[a];
    return out | 7seg_digit2[b];
}

void 7seg_loop(){ //one frame
    cycle = get_digit(7seg_buf[2],7seg_buf[3]);
    for(uint8_t i = 0; i<16; i++){
        data_out(cycle&1);
        cycle>>=1;
        pulse_clk();
    }
    data_out(0);
    select_digit(1);
    _delay_ms(1);
    select_digit(0);
    cycle = get_digit(7seg_buf[0],7seg_buf[1]);
    for(uint8_t i = 0; i<16; i++){
        data_out(cycle&1);
        cycle>>=1;
        pulse_clk();
    }
    data_out(0);
    select_digit(2);
    _delay_ms(1);
    select_digit(0);
}

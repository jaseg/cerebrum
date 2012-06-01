#include "config.h"
#ifdef HAS_7SEG_SUPPORT
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "7seg.h"

void l7seg_setup(){
    L7SEG_CLK_DDR |= _BV(L7SEG_CLK_PIN);
    L7SEG_DATA_DDR |= _BV(L7SEG_DATA_PIN);
    L7SEG_DIGIT1_DDR |= _BV(L7SEG_DIGIT1_PIN);
    L7SEG_DIGIT2_DDR |= _BV(L7SEG_DIGIT2_PIN);
}

inline void l7seg_pulse_clk(void){
    L7SEG_CLK_PORT |= _BV(L7SEG_CLK_PIN);
    L7SEG_CLK_PORT &= ~_BV(L7SEG_CLK_PIN);
}

inline void l7seg_data_out(uint8_t val){
    L7SEG_DATA_PORT &= ~_BV(L7SEG_DATA_PIN);
    if(!val){
        L7SEG_DATA_PORT |= _BV(L7SEG_DATA_PIN);
    }
}

inline void l7seg_select_digit(uint8_t digit){
    L7SEG_DIGIT1_PORT &= ~_BV(L7SEG_DIGIT1_PIN);
    L7SEG_DIGIT2_PORT &= ~_BV(L7SEG_DIGIT2_PIN);
    if(digit&1){ 
        L7SEG_DIGIT1_PORT |= _BV(L7SEG_DIGIT1_PIN);
    }
    if(digit&2){
        L7SEG_DIGIT2_PORT |= _BV(L7SEG_DIGIT2_PIN);
    }
}

int l7seg_cycle=1;

int l7seg_digit1[] = {
    0x56C0, //0
    0x0600, //1
    0x94C0, //2
    0x9640, //3
    0xC600, //4
    0xD240, //5
    0xD2C0, //6
    0x1600, //7
    0xD6C0, //8
    0xD640, //9
    0xD680, //A
    0xC2C0, //B
    0x50C0, //C
    0x86C0, //D
    0xD0C0, //E
    0xD080  //F
};

int l7seg_digit2[] = {
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

int l7seg_get_digit(uint8_t b, uint8_t a){
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
    out = l7seg_digit1[a];
    return out | l7seg_digit2[b];
}

void l7seg_loop(){ //one frame
    uint8_t nbuf = 0;
    static uint8_t pos = 0;
    if(pos&1){
        l7seg_cycle = l7seg_get_digit(l7seg_buf[2],l7seg_buf[3]);
        l7seg_select_digit(0);
        for(uint8_t i = 0; i<16; i++){
            l7seg_data_out(l7seg_cycle&1);
            l7seg_cycle>>=1;
            l7seg_pulse_clk();
        }
        l7seg_data_out(0);
        l7seg_select_digit(1);
    }else{
        l7seg_cycle = l7seg_get_digit(l7seg_buf[0],l7seg_buf[1]);
        l7seg_select_digit(0);
        for(uint8_t i = 0; i<16; i++){
            l7seg_data_out(l7seg_cycle&1);
            l7seg_cycle>>=1;
            l7seg_pulse_clk();
        }
        l7seg_data_out(0);
        l7seg_select_digit(2);
    }
    pos++;
}

uint8_t _l7seg_buf[4] = {'1','3','3','7'};
uint8_t* l7seg_buf = _l7seg_buf;

#else//HAS_7SEG_SUPPORT

void l7seg_setup(void){}
void l7seg_loop(void){}

#endif//HAS_7SEG_SUPPORT

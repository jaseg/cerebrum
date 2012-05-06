/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "r0ketbeam.h"

void setup(void);
void loop(void);

int main(void){
    setup();
    for(;;) loop();
}

void setup(){
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200, F_CPU));
    //s/w "BCM"(<== "Binary Code Modulation") timer setup
    /*
    TCCR0A |= _BV(WGM00)|_BV(WGM01);
    TCCR0B |= _BV(CS02);
    TIMSK0 |= _BV(TOIE0);
    */
    //FIXME set PWM output DDRs
    //The DDRs of the led matrix outputs are set in the mux loop.
    r0ketbeam_setup();
    sei();
}

//this scary construct is in place to make the compiler happy. if you know a better way, feel free to improve.
uint8_t _frameBuffer[] = {0,0,0,0};
uint8_t _secondFrameBuffer[] = {0,0,0,0};
uint8_t* frameBuffer = _frameBuffer;
uint8_t* secondFrameBuffer = _secondFrameBuffer;
char nbuf;
int state = 0;
uint8_t switch_last_state = 0;
int switch_debounce_timeout = 0;
uint8_t mcnt = 0;
uint8_t ccnt = 0;

#define PWM_COUNT 5
uint8_t pwm_cycle = 0;
uint8_t pwm_val[PWM_COUNT];
#define INPUT_COUNT 0
uint8_t debounce_timeouts[INPUT_COUNT];
uint8_t switch_states[INPUT_COUNT];

void swapBuffers(void){
    uint8_t* tmp = frameBuffer;
    frameBuffer = secondFrameBuffer;
    secondFrameBuffer = tmp;
}

void setLED(int num, int val){
    if(num<32){
        frameBuffer[num>>3] &= ~(1<<(num&7));
        if(val)
            frameBuffer[num>>3] |= 1<<(num&7);
    }
}

int parseHex(char* buf){
    int result = 0;
    int len = 2;
    for(int i=0; i<len; i++){
        char c = buf[len-i];
        int v = 0;
        if(c>='0' && c<='9'){
            v=(c-'0');
        }else if(c>='a' && c<= 'f'){
            v=(c-'a'+10);
        }else if(c>='A' && c<= 'F'){
            v=(c-'A'+10);
        }
        result |= v<<(4*i);
    }
    return result;
}

inline char hex_nibble(uint8_t data){
    if(data<0xA)
        return data+'0';
    else
        return data+'A'-0xA;
}

void put_hex(uint8_t data){
    uart_putc(hex_nibble(data&15));
    uart_putc(hex_nibble(data>>4));
}

void loop(){ //one frame
    static uint8_t escape_state = 0;
    uint16_t receive_state = 1;
    //primitive somewhat messy state machine of the uart interface
    do{ //Always empty the receive buffer since there are _delay_xxs in the following code and thus this might not run all that often.
        receive_state = uart_getc();
        char c = receive_state&0xFF;
        receive_state &= 0xFF00;
        //escape code format:
        // \\   - backslash
        // \n   - newline
        // \[x] - x
        //eats three commands: 's' (0x73) led value                     sets led number [led] to [value]
        //                     'b' (0x62) buffer buffer buffer buffer   sets the whole frame buffer
        //                     'a' (0x61) meter value                   sets analog meter number [meter] to [value]
        //                     'r' (0x72)                               read the frame buffer
        //this device will utter a "'c' (0x63) num state" when switch [num] changes state to [state]
        //commands are terminated by \n
        if(!receive_state){
            if(!escape_state){
                if(c == '\\'){
                    receive_state |= 0x02;
                    escape_state = 1;
                }else if(c == '\n'){
                    receive_state |= 0x02;
                    state = 0;
                }
            }else{
                receive_state = 0;
                escape_state = 0;
                switch(c){
                    case '\\':
                        break;
                    case 'n':
                        c = '\n';
                        break;
                }
            }
        }
        if(!receive_state){
            switch(state){
                case 0: //Do not assume anything about the variables used
                    //command char
                    switch(c){
                        case 's':
                            state = 2;
                            break;
                        case 'b':
                            nbuf = 0;
                            state = 4;
                            break;
                        case 'a':
                            state = 5;
                            nbuf = 0;
                            break;
                        case 'r':
                            uart_putc('r');
                            for(uint8_t i=0; i<sizeof(frameBuffer); i++){
                                put_hex(frameBuffer[i]);
                            }
                            uart_putc('\n');
                            break;
                    }
                    break;
                case 2:
                    nbuf=c;
                    state = 3;
                    break;
                case 3:
                    setLED(nbuf, c);
                    state = 0;
                    break;
                case 4:
                    secondFrameBuffer[(uint8_t) nbuf] = c;
                    nbuf++;
                    if(nbuf == 4){
                        swapBuffers();
                        state = 0;
                    }
                    break;
                case 5:
                    if(c > PWM_COUNT)
                        c = 0;
                    nbuf = c;
                    state = 6;
                    break;
                case 6:
                    pwm_val[(uint8_t) nbuf] = c;
                    state = 0;
            }
        }
    }while(!receive_state);
    for(int i=0; i<8; i++){
        uint8_t Q = 0x80>>i; //select the currently active "row" of the matrix. On the protoboards I make, this actually corresponds to physical traces.
        //uint8_t DDRQ = 0xFF>>i; //This is supposed to be an optimization. It is untested and will probably not work.
        uint8_t DDRQ = 0xFF; //Just for testing: reactivating the old behavioor
        //unpacking of frame data: data is packed like this: [11111117][22222266][33333555][4444----]
        if(!(i&4))
            Q |= frameBuffer[i&3] >> i;
        else
            Q |= frameBuffer[i&3] & (0xFF << (i&3));
        //FIXME this whole mapping shit should be done in h/w!!1!
        //FIXME this whole mapping is not even correct!!1!
        //FIXME IT DOES NOT WORK!!1!
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
        /* second channel skeleton
        Q = 0x80>>i;
        if(!(i&4))
            Q |= frameBuffer[4+(i&3)] >> i;
        else
            Q |= frameBuffer[4+(i&3)] & (0xFF << (i&3));
        DDRA = DDRQ;
        PORTA = Q;
        //PORTD &= 0x0F;
        //PORTD |= Q&0xF0;
        //PORTB &= 0xF0;
        //PORTB |= Q&0x0F;
        */
        _delay_ms(1);
    }
    r0ketbeam_loop();
    /* no switches (yet)
    switch_states[0] |= !!(PINH&0x02);
    for(int i=0; i<INPUT_COUNT; i++){
        debounce_timeouts[i]--;
        //A #define for the debounce time would be great
        if(debounce_timeouts[i] == 0){
            uint8_t new_switch_state = switch_states[i]<<1;
            if(!(switch_states[i]^new_switch_state)){
                uart_putc('c');
                uart_putc(i);
                uart_putc(switch_states[i]&1);
                debounce_timeouts[i] = 0xFF;
                switch_states[i] = new_switch_state&3;
            }
        }
    }
    */
}

//Software PWM stuff
//Called every 256 cpu clks (i.e should not get overly long)
ISR(TIMER0_OVF_vect){
    uint8_t Q = 0;
    Q |= (pwm_val[0] & pwm_cycle);
    Q |= (pwm_val[1] & pwm_cycle)?2:0;
    Q |= (pwm_val[2] & pwm_cycle)?4:0;
    Q |= (pwm_val[3] & pwm_cycle)?8:0;
    Q |= (pwm_val[4] & pwm_cycle)?16:0;
    PORTC = Q;
    OCR0A = pwm_cycle;
    pwm_cycle<<=1;
    if(!pwm_cycle)
        pwm_cycle = 1;
}

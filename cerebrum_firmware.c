/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "util.h"
#include "r0ketbeam.h"
#include "7seg.h"
#include "led.h"
#include "pwm.h"
#include "input.h"

void setup(void);
void loop(void);

int main(void){
    setup();
    for(;;) loop();
}

void setup(){
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200, F_CPU));
    pwm_setup();
    l7seg_setup();
    r0ketbeam_setup();
    input_setup();
    led_setup();
    config_setup();
    sei();
}

char nbuf;
char bpos;
int state = 0;
uint8_t somecycle = 0; //FIXME debug value

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
        //eats [n] commands:   's' (0x73) led value                         sets led number [led] to [value]
        //                     'b' (0x62) buffer buffer buffer buffer       sets the whole frame buffer
        //                     'a' (0x61) meter value                       sets analog meter number [meter] to [value]
        //                     'r' (0x72)                                   read the frame buffer
        //                     'd' (0x64) display digit digit digit digit   sets the 7-segment display (CAUTION: "display" currently is ignored)
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
#ifdef HAS_LED_SUPPORT
                        case 's':
                            state = 2;
                            break;
                        case 'b':
                            nbuf = 0;
                            state = 4;
                            break;
#endif//HAS_LED_SUPPORT
#ifdef HAS_PWM_SUPPORT
                        case 'a':
                            state = 5;
                            nbuf = 0;
                            break;
#endif//HAS_PWM_SUPPORT
#ifdef HAS_NOISE_MAKER_SUPPORT
                        case 'x':
                            //DDRF |= 0x01;
                            //PORTF |= 0x01;
                            break;
                        case 'X':
                            //DDRF &= 0xFE;
                            //PORTF &= 0xFE;
                            break;
#endif//HAS_NOISE_MAKER_SUPPORT
#ifdef HAS_LED_SUPPORT
                        case 'r':
                            uart_putc('r');
                            for(uint8_t i=0; i<sizeof(frameBuffer); i++){
                                uart_puthex(frameBuffer[i]);
                            }
                            uart_putc('\n');
                            break;
#endif//HAS_LED_SUPPORT
#ifdef HAS_7SEG_SUPPORT
                        case 'd':
                            nbuf = 0;
                            bpos = 0;
                            state = 7;
#endif//HAS_7SEG_SUPPORT
                    }
                    break;
#ifdef HAS_LED_SUPPORT
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
                    if(nbuf == 7){
                        swapBuffers();
                        state = 0;
                    }
                    break;
#endif//HAS_LED_SUPPORT
#ifdef HAS_PWM_SUPPORT
                case 5:
                    if(c > PWM_COUNT)
                        c = 0;
                    nbuf = c;
                    if(nbuf >= PWM_COUNT)
                        nbuf = 0;
                    state = 6;
                    break;
                case 6:
                    pwm_val[(uint8_t) nbuf] = c;
                    state = 0;
                    break;
#endif//HAS_PWM_SUPPORT
#ifdef HAS_7SEG_SUPPORT
                case 7:
                    nbuf = c;
                    state = 8;
                    break;
                case 8:
                    l7seg_buf[(uint8_t)bpos] = c;
                    bpos++;
                    if(bpos == 4){
                        state = 0;
                    }
                    break;
#endif//HAS_7SEG_SUPPORT
            }
        }
    }while(!receive_state);
    led_loop();
    r0ketbeam_loop();
    l7seg_loop();
    input_loop();
    pwm_loop();
    _delay_ms(1);
    somecycle++;
    if(!somecycle){
        pwm_val[0]++;
        pwm_val[1]++;
        pwm_val[2]++;
        pwm_val[3]++;
    }
}

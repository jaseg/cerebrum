
#include "config.h"
#include "pwm.h"
#include "input.h"
#include "uart.h"
#include "util.h"

hsv_value_t current_color;
uint8_t current_component = 0;

void config_setup(){
    //input setup
    PORTD |= 0x04; //green/up button
    PORTB |= 0x04; //red/down button
    DDRD  |= 0x80; //switch ground output
    PORTC |= 0x02; //channel switch position 1
    PORTD |= 0x08; //channel switch position 2
}

void pwm_output_setup(){
    DDRD |= 0x70;
}

void pwm_unset_outputs(){
    PORTD &= 0x8F;
}

void pwm_set_outputs(uint8_t mask){
    PORTD |= (mask&7)<<4;
}

void config_loop(){
    static uint8_t ct = 0;
    //channel selection switch
    current_component = 0;
    if(!(PINC & 0x02)){
        current_component |= 1;
    }
    if(!(PIND & 0x08)){
        current_component |= 2;
    }
    if(!ct){
        ct = 25;
        if(!(PIND&0x04)){ //up button
            switch(current_component){
                case 0:
                    current_color.h++;
                    current_color.h%=360;
                    break;
                case 1:
                    if(current_color.s < 255)
                        current_color.s++;
                    break;
                case 2:
                    if(current_color.v < 255)
                    current_color.v++;
                    break;
            }
        }else{
            if(!(PINB&0x04)){ //down button
                switch(current_component){
                    case 0:
                        current_color.h--;
                        current_color.h%=360;
                        break;
                    case 1:
                        if(current_color.s > 0)
                            current_color.s--;
                        break;
                    case 2:
                        if(current_color.v > 0)
                            current_color.v--;
                        break;
                }
            }else{
                return;
            }
        }
        //update the pwm color
        //FIXME this needs to be tested a) for correctness of the used conversion function b) for speed of the used conversion function
        rgb_value_t newval = hsv_to_rgb(current_color);
        uart_putc('#');
        uart_puthex(newval.r);
        uart_puthex(newval.g);
        uart_puthex(newval.b);
        uart_putc(' ');
        uart_puthex(current_component);
        uart_putc('\n');
        pwm_val[0] = newval.r;
        pwm_val[1] = newval.g;
        pwm_val[2] = newval.b;
    }else{
        ct--;
    }
}


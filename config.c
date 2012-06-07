
#include "config.h"
#include <avr/io.h>
#include "input.h"

void config_setup(){
}

void pwm_output_setup(){
    DDRD |= 0x04;
}

void pwm_unset_outputs(){
    PORTD &= ~0x04;
}

void pwm_set_outputs(uint8_t data){
    data <<= 2;
    PORTD |= data&0x04;
}


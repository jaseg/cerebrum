
#include "config.h"
#include <avr/io.h>
#include "input.h"

void config_setup(){
    PORTH |= 0x20;
}

void input_scan_inputs(){
    switch_states[0] |= !!(PINH&0x20);
}

void led_output_stuff1(uint8_t data, uint8_t dir){
    PORTK = data;
    DDRK = dir;
}


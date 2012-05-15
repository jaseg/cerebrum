
#include "input_config.h"
#include "inputs.h"

uint8_t switch_last_state = 0;
int switch_debounce_timeout = 0;

void input_setup(void){
    //FIXME set input DDRs
}

void input_loop(void){
    //FIXME scan inputs
    //switch_states[0] |= !!(PINH&0x02);
    for(int i=0; i<INPUT_COUNT; i++){
        debounce_timeouts[i]--;
        //A #define for the debounce time would be great
        if(debounce_timeouts[i] == 0){
            uint8_t new_switch_state = switch_states[i]<<1;
            if(!(switch_states[i]^new_switch_state)){
                uart_putc('c');
                uart_puthex(i);
                uart_puthex(switch_states[i]&1);
                debounce_timeouts[i] = 0xFF;
                switch_states[i] = new_switch_state&3;
            }
        }
    }
}


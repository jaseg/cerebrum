
#include "input.h"
#ifdef HAS_INPUT_SUPPORT

void input_setup(void){
}

void input_loop(void){
    input_scan_inputs();
    for(int i=0; i<INPUT_COUNT; i++){
        //A #define for the debounce time would be great
        if(debounce_timeouts[i] == 0){
            if(switch_states[i]&1 ^ switch_states[i]>>1){
                uart_putc('c');
                uart_puthex(i);
                uart_puthex(switch_states[i]&1);
                uart_putc('\n');
                input_callback(i, switch_states[i]&1);
                debounce_timeouts[i] = 0xF0;
                switch_states[i] = (switch_states[i]<<1)&3;
            }
            switch_states[i] &= 0xFE;
        }else{
            debounce_timeouts[i]--;
        }
    }
}

uint8_t debounce_timeouts[INPUT_COUNT];
uint8_t switch_states[INPUT_COUNT];

#else//HAS_INPUT_SUPPORT

void input_setup(){}
void input_loop(){}

#endif//HAS_INPUT_SUPPORT

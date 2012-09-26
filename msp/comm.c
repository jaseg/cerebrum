
#include <stdint.h>
#include <stddef.h>
#include <msp430.h>
#include "comm.h"
#include "uart_io.h"

void comm_loop(){
    static uint8_t escape_state = 0;
    static uint16_t pos = 0;
    static uint8_t command = 0;
    static uint16_t address = 0;
    static uint8_t length = 0;
    uint16_t v = uart_getc_nonblocking();
    uint8_t c = v&0xff;
    v &= 0xff00;
    if(!v){ //a character was successfully received
        if(escape_state){
            if(c == '#'){
                command = 0;
                pos = 0xff;
                return;
            }
            escape_state = 0;
        }else{
            if(c == '\\'){
                escape_state = 1;
                return;
            }
        }
        if(pos == 0){
            command = c;
        }else{
            switch(command){
                case 1:
                    switch(pos){
                        case 1:
                            address = c<<8;
                            break;
                        case 2:
                            address |= c;
                            break;
                        case 3:
                            comm_set_reg(address, c);
                            break;
                    }
                    break;
                case 2:
                    switch(pos){
                        case 1:
                            address = c<<8;
                            break;
                        case 2:
                            address |= c;
                            putc_escaped(comm_get_reg(address));
                            break;
                    }
                    break;
                case 3:
                    switch(pos){
                        case 1:
                            address = c<<8;
                            break;
                        case 2:
                            address |= c;
                            break;
                        case 3:
                            length = c;
                            pos = 0x0100;
                            break;
                        default:
                            if(pos&0xFF < length){
                               comm_set_reg(address, c);
                               address++;
                            }
                            break;
                    }
                    break;
                case 4:
                    switch(pos){
                        case 1:
                            address = c<<8;
                            break;
                        case 2:
                            address |= c;
                            break;
                        case 3:
                            for(uint16_t i=0; i<=c; i++){
                                putc_escaped(comm_get_reg(address));
                                address++;
                            }
                        break;
                    }
            }
        }
        if(pos < 0xFFFF){
            pos++;
        }
    }
}

void putc_escaped(char c){
    if(c == '\\'){
        uart_putc(c);
    }
    uart_putc(c);
}

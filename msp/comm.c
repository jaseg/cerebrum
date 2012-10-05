
#include <stdint.h>
#include <stddef.h>
#include <msp430.h>
#include "comm.h"
#include "uart_io.h"

void comm_loop(){
    static uint8_t escape_state = 0;
	static uint8_t state = 0;
    static uint16_t pos = 0;
    static uint16_t funcid = 0;
	static uint16_t crcbuf = 0;
	static uint8_t argbuf[ARGBUF_SIZE];
	static uint16_t arglen = 0;

    uint16_t v = uart_getc_nonblocking();
    uint8_t c = v&0xff;
    v &= 0xff00;
    if(!v){ //a character was successfully received
        if(escape_state){
            if(c == '#'){ //FIXME
                pos = 0;
                return;
            }
            escape_state = 0;
        }else{
            if(c == '\\'){
                escape_state = 1;
                return;
            }
        }
		//escape sequence handling completed. 'c' now contains the next char of the payload.
		switch(state){
			case 0: //receive funcid, payload length
				switch(pos){
					case 0:
						funcid = c<<8;
						pos++;
						break;
					case 1:
						funcid |= c;
						pos++;
						break;
					case 2:
						arglen = c<<8;
						pos++;
						break;
					case 3:
						pos = c;
						arglen |= c;
						state = 1;
						break;
				}
				break;
			case 1: //receive arg payload
				pos--;
				argbuf[pos] = c;
				if(pos == 0)
					state = 3;
				break;
			case 2: //receive and check payload crc. finally, call the function and send the return value
				if(pos == 0){
					crcbuf = c<<8;
				}else{
					//successfully received the crc
					//FIXME add crc checking
					if(funcid < NUM_CALLBACKS){
						//HACK: The argument buffer is currently passed as the response buffer for ram efficiency.
						//Only write to the response buffer *after* you are done reading from it.
						comm_callbacks[funcid](arglen, argbuf);
					}else{
						//FIXME error handling
					}
				}
				break;
		}
    }
}

void putc_escaped(char c){
    if(c == '\\'){
        uart_putc(c);
    }
    uart_putc(c);
}

/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <stdint.h>
#include <stddef.h>
#include "comm.h"
#include "uart.h"

#ifdef TEST
#include <stdio.h>
#define comm_debug_print printf

#else
#define comm_debug_print(...)
#endif//TEST

int16_t comm_loop(){
    static uint8_t escape_state = 0;
	static uint8_t state = 0xFF; //"idle" state
    static uint16_t pos = 0;
    static uint16_t funcid = 0;
	static uint16_t crcbuf = 0;
	static uint8_t argbuf[ARGBUF_SIZE];
	static uint16_t arglen = 0;
	static uint16_t payload_offset = 0;

    int16_t v = uart_getc();
    uint8_t c = v&0xff;
    v &= 0xff00;
    if(!v){ //a character was successfully received
        comm_debug_print("[DEBUG] received %c\n", c);
        if(escape_state){
            if(c == '#'){
                comm_debug_print("[DEBUG] starting message\n");
                pos = 0;
				state = 0;
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
		if(state == 1){ //receive arg payload
            comm_debug_print("[DEBUG] receiving byte %d\n", pos);
			argbuf[pos] = c;
            uart_putc(pos);
			pos++;
			if(pos == arglen || pos == ARGBUF_SIZE){
                comm_debug_print("[DEBUG] recv buffer full\n");
				state = 2; //CAUTION this state is still handled in *this* iteration
			}
		}else{
            //at this point, state would better be 0
			if(state == 0){//receive funcid, payload length
                comm_debug_print("[DEBUG] receiving message header byte %d\n", pos);
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
						arglen |= c;
                        uart_putc(arglen>>8);
                        uart_putc(arglen&0xFF);
						pos = 0;
						if(arglen == 0)
							state = 2; //CAUTION this state is still handled in *this* iteration
						else
							state = 1;
						break;
				}
			}
		}
		//receive and check payload crc. finally, call the function and send the return value
        //CAUTION this block may be entered even after this iteration began with a state of 0 or 1.
		if(state == 2){
			if(pos == arglen && arglen > 0){ // arrived at the end of the message. do not expect a crc when the message was 0 bytes long.
                comm_debug_print("[DEBUG] receiving crc byte 0\n");
				crcbuf = c<<8;
				pos++; //HACK
			}else{
				if(pos > arglen){ //only entered when in the previous iteration the msb of the crc was received in the if clause above.
                    comm_debug_print("[DEBUG] receiving crc byte 1\n");
					crcbuf |= c;
					//successfully received the crc
					//FIXME add crc checking
				}
				if(funcid < (sizeof(comm_callbacks)/sizeof(comm_callback))){ //only jump to valid callbacks.
                    comm_debug_print("[DEBUG] calling callback %d with offset %d and len %d\n", funcid, payload_offset, arglen);
					comm_callbacks[funcid](payload_offset, arglen<ARGBUF_SIZE?arglen:ARGBUF_SIZE, argbuf);
				}else{
					//FIXME error handling
				}
				if(arglen > ARGBUF_SIZE){
                    comm_debug_print("[DEBUG] increasing offset from %d, going for another round\n", payload_offset);
					state = 1;
					pos = 0;
                    payload_offset += ARGBUF_SIZE;
					arglen -= ARGBUF_SIZE;
				}else{
                    comm_debug_print("[DEBUG] going to idle state\n");
					state = 0xFF; //msg successfully received. go to "idle" state.
				}
			}
		}
    }
    return v;
}

void putc_escaped(char c){
    if(c == '\\'){
        uart_putc(c);
    }
    uart_putc(c);
}

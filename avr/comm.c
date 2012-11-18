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

void comm_loop(){
    static uint8_t escape_state = 0;
	static uint8_t state = 0xFF;
    static uint16_t pos = 0;
    static uint16_t funcid = 0;
	static uint16_t crcbuf = 0;
	static uint8_t argbuf[ARGBUF_SIZE];
	static uint16_t arglen = 0;
	static uint16_t payload_offset = 0;

    uint16_t v = uart_getc();
    uint8_t c = v&0xff;
    v &= 0xff00;
    if(!v){ //a character was successfully received
        if(escape_state){
            if(c == '#'){
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
			argbuf[pos] = c;
			pos++;
			if(pos == arglen)
				state = 2;
			if(pos == ARGBUF_SIZE){
				payload_offset += ARGBUF_SIZE;
				state = 2;
			}
		}else{
            //at this point, state would better be 0
			if(state == 0){//receive funcid, payload length
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
						pos = 0;
						if(arglen == 0)
							state = 2;
						else
							state = 1;
						break;
				}
			}
		}
		//receive and check payload crc. finally, call the function and send the return value
		if(state == 2){
			if(pos == arglen && arglen > 0){
				crcbuf = c<<8;
				pos++;
			}else{
				if(pos > arglen){
					crcbuf |= c;
					//successfully received the crc
					//FIXME add crc checking
				}
				if(funcid < num_callbacks){
					comm_callbacks[funcid](payload_offset, arglen<ARGBUF_SIZE?arglen:ARGBUF_SIZE, argbuf);
				}else{
					//FIXME error handling
				}
				if(arglen > ARGBUF_SIZE){
					state = 1;
					pos = 0;
					arglen -= ARGBUF_SIZE;
				}else{
					state = 0xFF;
				}
			}
		}
    }
}

void putc_escaped(char c){
    if(c == '\\'){
        uart_putc(c);
    }
    uart_putc(c);
}

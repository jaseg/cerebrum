/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <stdint.h>
#include <stddef.h>
#include "comm.h"

#ifdef __TEST__
#include <stdio.h>
#define comm_debug_print(...) //fprintf(stderr, __VA_ARGS__)
#define comm_debug_print2(...) //fprintf(stderr, __VA_ARGS__)

#else
#define comm_debug_print(...)
#define comm_debug_print2(...)
#endif//TEST

void comm_handle(uint8_t c){
    static uint8_t escape_state = 0;
	static uint8_t state = 0xFF; //"idle" state
    static uint16_t pos = 0;
    static uint16_t funcid = 0;
	static uint8_t* argbuf;
	static uint16_t arglen = 0;
	static uint16_t payload_offset = 0;

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
	if(state = 0xFF){//idle state
		return;
	}
    //escape sequence handling completed. 'c' now contains the next char of the payload.
    if(state == 1){ //receive arg payload
        comm_debug_print("[DEBUG] receiving byte %d\n", pos);
        argbuf[pos] = c;
        pos = pos + 1;
        if(pos == arglen || pos == ARGBUF_SIZE){
            comm_debug_print("[DEBUG] recv buffer full\n");
            state = 2; //CAUTION this state is still handled in *this* iteration
        }else{
			return;
		}
    }else if(state == 0){//at this point, state would better be 0
		//receive funcid, payload length
		comm_debug_print("[DEBUG] receiving message header byte %d\n", pos);
		switch(pos){
			case 0:
				funcid = c<<8;
				pos++;
				return;
			case 1:
				funcid |= c;
				pos++;
				return;
			case 2:
				arglen = c<<8;
				pos++;
				return;
			case 3:
				arglen |= c;
				pos = 0;
				payload_offset = 0;
				if(arglen == 0){
					state = 2; //CAUTION this state is still handled in *this* iteration
				}else{
					state = 1;
					return;
				}
		}
    }
   	//CAUTION at this point, state is *ALWAYS* 2
	if(funcid < callback_count){ //only jump to valid callbacks.
		comm_debug_print2("[DEBUG] calling callback %d with offset %d and len %d\n", funcid, payload_offset, arglen);
		comm_callbacks[funcid](payload_offset, arglen<ARGBUF_SIZE?arglen:ARGBUF_SIZE, argbuf);
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


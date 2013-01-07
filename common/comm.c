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
#else//__TEST__
#define comm_debug_print(...)
#define comm_debug_print2(...)
#endif//__TEST__

uint8_t global_argbuf[ARGBUF_SIZE];

void comm_handle(uint8_t c){
	static struct state {
		uint8_t receiving:1;
		uint8_t escaped:1;
	};
	static struct args {
		uint16_t funcid;
		uint16_t arglen;
	};
	static void* argbuf;
	static void* argbuf_end;
#define ARGS_END (&args+sizeof(args))
    if(state.escaped){
        if(c == '#'){
            comm_debug_print("[DEBUG] starting message\n");
            state.receiving = 1;
			argbuf = &args;
			argbuf_end = ARGS_END;
            return;
        }
        state.escaped = 0;
    }else{
        if(c == '\\'){
            state.escaped = 1;
            return;
        }
    }
	//escape sequence handling completed. 'c' now contains the next char of the payload.
	if(!state.receiving){
		return;
	}
	*argbuf = c;
	argbuf++;
	if(argbuf == argbuf_end){
		if(argbuf_end == ARGS_END){
			comm_debug_print("[DEBUG] received the header\n");
			argbuf = comm_callbacks[args.funcid].argbuf;
			uint16_t len = comm_callbacks[args.funcid].argbuf_len;
			if(args.arglen < len)
				len = args.arglen;
			argbuf_end = argbuf+len;
		}else{
			comm_debug_print("[DEBUG] received the body\n");
			if(args.funcid < callback_count){ //only jump to valid callbacks.
				comm_debug_print2("[DEBUG] calling callback %d\n", args.funcid);
				comm_callback cb = comm_callbacks[args.funcid].callback;
				if(cb != 0){
					uint16_t len = comm_callbacks[args.funcid].argbuf_len;
					if(args.arglen < len)
						len = args.arglen;
					cb(len);
				}
			}
			comm_debug_print("[DEBUG] going to idle state\n");
			state.receiving = 0;
		}
	}
#undef ARGS_END
}


#ifndef COMM_HANDLE_H
#define COMM_HANDLE_H

#include "comm.h"
#ifdef __TEST__
#include <stdio.h>
#include <endian.h> //FIXME detect endianness
#define comm_debug_print(...) //fprintf(stderr, __VA_ARGS__)
#define comm_debug_print2(...) //fprintf(stderr, __VA_ARGS__)
#else//__TEST__
#define htobe16(...) (__VA_ARGS__)
#define comm_debug_print(...)
#define comm_debug_print2(...)
#endif//__TEST__

static inline void comm_handle(uint8_t c){
	typedef struct {
		uint8_t receiving:1;
		uint8_t escaped:1;
	} state_t;
	typedef struct {
		uint16_t funcid;
		uint16_t arglen;
	} args_t;
	static state_t state;
	static void* argbuf;
	static void* argbuf_end;
	static comm_callback_descriptor* current_callback;
	args_t* args = (args_t*)global_argbuf;
#define ARGS_END (((uint8_t*)(args))+sizeof(args_t))
    //comm_debug_print("[DEBUG] received %c\n", c);
    /*uart_putc_nonblocking('A');
    uart_putc_nonblocking(c);
    uart_putc_nonblocking(state.escaped);
    uart_putc_nonblocking(state.receiving);*/
	if(state.escaped){
        state.escaped = 0;
		if(c == '#'){
			state.receiving = 1;
			argbuf = args;
			argbuf_end = ARGS_END;
            comm_debug_print("[DEBUG] starting message, argbuf @0x%x, argbuf_end @0x%x\n", argbuf, argbuf_end);
			return;
		}
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
	*(uint8_t*)argbuf = c;
	argbuf++;
    comm_debug_print("[DEBUG] stored 0x%x at 0x%x, end at 0x%x\n", c, argbuf, argbuf_end);
	if(argbuf == argbuf_end){
		if(argbuf_end == ARGS_END){
			comm_debug_print("[DEBUG] received the header\n");
			if(htobe16(args->funcid) >= callback_count){ //only jump to valid callbacks.
                comm_debug_print("[DEBUG] invalid callback: funcid=0x%x given, callback_count=0x%x\n", htobe16(args->funcid), callback_count);
                state.receiving = 0; //return to idle state
                return;
            }
            comm_debug_print("[DEBUG] getting the callback\n");
            argbuf = comm_callbacks[htobe16(args->funcid)].argbuf;
            current_callback = comm_callbacks + htobe16(args->funcid);
            uint16_t len = current_callback->argbuf_len;
            if(htobe16(args->arglen) <= len){
                len = htobe16(args->arglen);
            }
            argbuf_end = argbuf+len;
            if(argbuf != argbuf_end){
                return;
            }
		}
        comm_debug_print2("[DEBUG] calling callback\n");
        state.receiving = 0;
        if(current_callback->callback != 0){
			next_callback.argbuf_end = argbuf_end;
			next_callback.descriptor = current_callback;
            //(*current_callback->callback)(current_callback, argbuf_end);
        }else{
            //Send a minimal response.
            uart_putc_nonblocking(0);
            uart_putc_nonblocking(0);
        }
        comm_debug_print("[DEBUG] going to idle state\n");
    }
#undef ARGS_END
}

#endif//COMM_HANDLE_H


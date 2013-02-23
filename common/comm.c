/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <stdint.h>
#include <stddef.h>
#include "comm.h"

const volatile uint8_t global_argbuf[ARGBUF_SIZE];
volatile callback_stack_t next_callback;

void comm_loop(){
	if(next_callback.descriptor){
		(*next_callback.descriptor->callback)(next_callback.descriptor, next_callback.argbuf_end);
		next_callback.descriptor = 0;
	}
}


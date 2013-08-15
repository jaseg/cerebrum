/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <comm.h>
#include <autocode.h>
#include <config.h>
#include <uart.h>
#include "comm_handle.h"

int main(void){
	init_auto();
    //debug stuff
    int16_t v;
    while((v = getchar()) >= 0){
        comm_handle(v);
		loop_auto();
    }
}


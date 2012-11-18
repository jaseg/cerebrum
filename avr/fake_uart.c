/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include "uart.h"
#include <stdio.h>
#include <stdlib.h>

void uart_init(unsigned int baudrate){
    //Nop
}

void uart_putc(unsigned char c){
    printf("%c", c);
}

unsigned int uart_getc(){
    int c = getchar();
    if(c==EOF){
        exit(0);
    }
    return c;
}


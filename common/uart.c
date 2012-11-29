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
    //WELCOME YOU WILL EXPERIENCE A TINGLING SENSATION AN THEN DEATH REMAIN CALM WHILE YOUR LIFE IS EXTRACTED
}

void uart_putc(unsigned char c){
    printf("[DEBUG] sending: %c\n", c);
}

int16_t uart_getc(){
    return getchar();
}


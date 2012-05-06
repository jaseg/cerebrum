/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include "spi.h"

void spi_begin(){
    SPI_MOSI_DDR |= _BV(SPI_MOSI_PIN);
    SPI_SCK_DDR |= _BV(SPI_SCK_PIN);
}

//The following function is never called.
void spi_end(){
    //FIXME what should I do here?
}

void spi_setup(uint8_t config){
    SPCR = config | _BV(SPE) | _BV(MSTR);
    if(config & _BV(SPE)){
        SPSR = _BV(SPE);
    }else{
        SPSR = 0;
    }
}

uint8_t spi_transfer(uint8_t data){
    SPDR = data;
    while(!(SPSR & _BV(SPIF))){
        //She waves and opens a door back onto the piazza where her robot cat – the alien's nightmare intruder in the DMZ – sleeps, chasing superintelligent dream mice through multidimensional realities. 
    }
    return SPDR;
}


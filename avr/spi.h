/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h>

#define SPI_MOSI_PORT   PORTB
#define SPI_MOSI_DDR    DDRB
#define SPI_MOSI_PIN    2

#define SPI_SCK_PORT    PORTB
#define SPI_SCK_DDR     DDRB
#define SPI_SCK_PIN     1

void spi_begin(void);

void spi_end(void);

void spi_setup(uint8_t config);
#define SPI_MSBFIRST 0
#define SPI_LSBFIRST _BV(DORD)
#define SPI_MODE0 0
#define SPI_MODE1 _BV(CPHA)
#define SPI_MODE2 _BV(CPOL)
#define SPI_MODE3 (_BV(CPHA) | _BV(CPOL))
//To avoid passing more than 1 byte to the setup function, the SPI2X bit is mapped onto the SPE bit here
#define SPI_CLOCK_DIV2 _BV(SPE)
#define SPI_CLOCK_DIV4 0
#define SPI_CLOCK_DIV8 (_BV(SPE) | _BV(SPR0))
#define SPI_CLOCK_DIV16 _BV(SPR0)
#define SPI_CLOCK_DIV32 (_BV(SPE) | _BV(SPR1))
#define SPI_CLOCK_DIV64 _BV(SPR1)
#define SPI_CLOCK_DIV128 (_BV(SPR1) | _BV(SPR0))

uint8_t spi_transfer(uint8_t data);

#endif//__SPI_H__

/*
* Copyright (c) 2012, Alexander I. Mykyta
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer. 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* \addtogroup MOD_UART UART IO
* \brief Provides basic text IO functions for the MSP430 UART controller
* \author Alex Mykyta (amykyta3@gmail.com)
*
* This module abstracts out the UART controller to provide basic text input and output functions. \n
*
* <b> MSP430 Processor Families Supported: </b>
*	- MSP430x2xx
*	- MSP430x5xx
*	- MSP430x6xx
*
* <b> Compilers Supported: </b>
*	- TI CCS v4
*	- MSPGCC
*
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_UART "UART IO"
* \author Alex Mykyta (amykyta3@gmail.com)
**/

#ifndef __UART_IO_H__
#define __UART_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "uart_io_config.h"

//==================================================================================================
// Function Prototypes
//==================================================================================================
///\addtogroup UART_FUNCTIONS Functions
///\{
void init_uart(void);

uint8_t getc(void);		// Returns the next character recieved (Blocking function)
char* getline(char *line);	// Gets a string from input until it hits a '\n' (Blocking Function)
uint16_t incount(void);	// Returns the number of characters immediately available for input

void putc(uint8_t c);		// Sends a character to the serial port.
void puts(char *s);		// Sends a string
void putx(uint16_t value, uint16_t places);	// Sends a value formatted in Hex
void putd(uint16_t value);	// Sends an unsigned value formatted in decimal
void putsd(int16_t value);	// Sends a signed value formatted in decimal
void putd32(uint32_t value);
///\}

#ifdef __cplusplus
}
#endif

#endif
///\}

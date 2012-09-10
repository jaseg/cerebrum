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

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       11/15/2010   born
* 
*=================================================================================================*/

/**
* \addtogroup MOD_UART
* \{
**/

/**
* \file
* \brief Code for \ref MOD_UART "UART IO"
* \author Alex Mykyta (amykyta3@gmail.com)
**/

#include <stdint.h>
#include <stddef.h>

#include <msp430.h>
#include "result.h"
#include "uart_io.h"
#include "uart_io_internal.h"
#include "fifo.h"

///\cond PRIVATE
#if (UIO_USE_INTERRUPTS == 1)
	char rxbuf[UIO_RXBUF_SIZE];
	FIFO_t RXFIFO;
	char txbuf[UIO_TXBUF_SIZE];
	FIFO_t TXFIFO;
	
	uint8_t	txbusy;
#endif
///\endcond

///\addtogroup UART_FUNCTIONS
///\{

//==================================================================================================
// Hardware Abstraction Layer
//==================================================================================================
/**
* \brief Initializes the UART controller
* \param None
* \return Nothing
* \attention The initialization routine does \e not setup the IO ports!
**/
void init_uart(void){
#if defined(__MSP430_HAS_USCI__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	UIO_CTL1 |= UCSWRST; // soft reset
	UIO_CTL0 = 0;
	UIO_CTL1 = (UIO_CLK_SRC*64)+UCSWRST;
	UIO_BR0	= UIO_BR0_DEFAULT;
	UIO_BR1	= UIO_BR1_DEFAULT;
	UIO_MCTL = UIO_MCTL_DEFAULT;
	
	UIO_CTL1 &= ~UCSWRST;
	
#if (UIO_USE_INTERRUPTS == 1)
	fifo_init(&RXFIFO,rxbuf,UIO_RXBUF_SIZE);
	fifo_init(&TXFIFO,txbuf,UIO_TXBUF_SIZE);
	txbusy = 0;
#if (UIO_ISR_SPLIT == 0)
	UIO_IE = UCRXIE;
#else
	UIO_IE = UIO_UCARXIE;
#endif
#endif

#elif defined(__MSP430_HAS_UCA__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#error "Hardware Abstraction for UCA not written yet!"
#endif
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Gets a character from the input stream
* \details If a character is not immediately available, function will block until it receives one.
* \param None
* \retval uint8_t Character
**/
uint8_t getc(void){
#if defined(__MSP430_HAS_USCI__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if (UIO_USE_INTERRUPTS == 1)
	uint8_t chr;
	while(fifo_rdcount(&RXFIFO) == 0); // wait until char recieved
	fifo_read(&RXFIFO,&chr,1);
	return(chr);
#else
	while((UIO_IFG & UCRXIFG) == 0); // wait until char recieved
	return(UIO_RXBUF);
#endif

#elif defined(__MSP430_HAS_UCA__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#error "Hardware Abstraction for UCA not written yet!"
#endif
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Returns the number of characters immediately available for input
* \param None
* \retval uint16_t Characters available
**/
uint16_t incount(void){
#if defined(__MSP430_HAS_USCI__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if (UIO_USE_INTERRUPTS == 1)
	return(fifo_rdcount(&RXFIFO));
#else
	if((UIO_IFG & UCRXIFG) != 0){
		return(1);
	}
	else{
		return(0);
	}
#endif

#elif defined(__MSP430_HAS_UCA__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#error "Hardware Abstraction for UCA not written yet!"
#endif
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Sends a character
* \param [in] c Character to be sent
* \return Nothing
**/
void putc(uint8_t c){
#if defined(__MSP430_HAS_USCI__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if (UIO_USE_INTERRUPTS == 1)
	uint8_t chr;
	while(fifo_wrcount(&TXFIFO) == 0); // wait until fifo has room for another
	fifo_write(&TXFIFO,&c,1);
	
	if(txbusy == 0){
		txbusy = 1;
		fifo_read(&TXFIFO,&chr,1);
		UIO_TXBUF = chr;
		#if (UIO_ISR_SPLIT == 0)
		UIO_IE |= UCTXIE;
		#else
		UIO_IE |= UIO_UCATXIE;
		#endif
		
	}
#else
	while((UIO_IFG & UCTXIFG) == 0); // wait until txbuf is empty
	UIO_TXBUF = c;
#endif

#elif defined(__MSP430_HAS_UCA__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#error "Hardware Abstraction for UCA not written yet!"
#endif
}

//--------------------------------------------------------------------------------------------------

///\cond PRIVATE
#if defined(__MSP430_HAS_USCI__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if (UIO_USE_INTERRUPTS == 1)
#if (UIO_ISR_SPLIT == 0)
// RX/TX Interrupt Service Routine
ISR(UIO_ISR_VECTOR, UIO_rxtxISR){

	uint16_t iv = UIO_IV;
	uint8_t chr;
	if(iv == 0x02){
		// Data Recieved
		chr = UIO_RXBUF;
		fifo_write(&RXFIFO,&chr,1);
	}else if(iv == 0x04){
		// Transmit Buffer Empty
		if(fifo_read(&TXFIFO,&chr,1) == RES_OK){
			UIO_TXBUF = chr;
		}else{
			txbusy = 0;
			UIO_IE &= ~UCTXIE; // disable tx interrupt
		}
	}
}
#else
// RX Interrupt Service Routine
ISR(UIO_RXISR_VECTOR, UIO_rxISR){
	uint8_t chr;
	chr = UIO_RXBUF;
	fifo_write(&RXFIFO,&chr,1);
}

// TX Interrupt Service Routine
ISR(UIO_TXISR_VECTOR, UIO_txISR){
	uint8_t chr;
	if(fifo_read(&TXFIFO,&chr,1) == RES_OK){
			UIO_TXBUF = chr;
	}else{
		txbusy = 0;
		UIO_IE &= ~UIO_UCATXIE; // disable tx interrupt
	}
}
#endif
#endif

#elif defined(__MSP430_HAS_UCA__)	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#error "Hardware Abstraction for UCA not written yet!"
#endif

///\endcond

//==================================================================================================
//   General functions
//==================================================================================================
/**
* \brief Sends a string to the serial port
* \param [in] s Pointer to string to be sent
* \return Nothing
**/
void puts(char *s) {
	while(*s) {
		if(*s == '\n')
			putc('\r');
		putc(*s++);
	}
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Reads in a string until a newline character ( \c \n)is received
* \details If an entire is not immediately available, function will block until it receives a
*	newline character.
* \param [in] line Pointer to line buffer to be used. Buffer /e MUST be large enough to receive
*	the line!
* \return Pointer to the received string
**/
char* getline(char *line){
	char *ch = line;
	uint8_t k;
	
	// until we read a newline
    while ((k = getc()) != '\n') {
		*ch++ = k;
	}
	
	// newline dropped and Null-terminating character added
    *ch = '\0';
	
	// return original pointer
    return(line); 
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Prints a HEX formatted string
* \param [in] value Value to be printed
* \param [in] places Number of digits to print. Use \c 0 to automatically determine how many digits.
* \return Nothing
**/
void putx(uint16_t value, uint16_t places){
	int16_t i;
	uint16_t j;
	uint16_t bitmask;
	if(places == 0){
		// size places to fit number
		bitmask = 0xF000;
		places = 4;
		while(((value & bitmask) == 0) && (places > 0)){
			places--;
			bitmask = bitmask >> 4;
		}
		if(places == 0){
			places = 1;
		}
	}
	for(i = ((places-1)<<2); i >= 0; i -= 4) {
		j = (value >> i) & 0xf;
		if(j < 10) 
			putc('0' + j);
		else 
			putc('A' - 10 + j);
	}
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Prints an unsigned decimal formatted string
* \param [in] value Value to be printed
* \return Nothing
**/
void putd(uint16_t value){
  uint16_t n;
  uint8_t str[5];
  n = 5;

  do{
    n -=1;
    str[n]=(value % 10) + '0';
    value /= 10;
  } while(value != 0);

  do{
    putc(str[n]);
    n++;
  } while(n<5);
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Prints an unsigned decimal formatted string (32 bit version)
* \param [in] value Value to be printed
* \return Nothing
**/
void putd32(uint32_t value){
  uint16_t n;
  uint8_t str[10];
  n = 10;

  do{
    n -=1;
    str[n]=(value % 10) + '0';
    value /= 10;
  } while(value != 0);

  do{
    putc(str[n]);
    n++;
  } while(n<10);
}
//--------------------------------------------------------------------------------------------------
/**
* \brief Prints a signed decimal formatted string
* \param [in] value Value to be printed
* \return Nothing
**/
void putsd(int16_t value){
  uint16_t n;
  uint16_t uvalue;
  uint8_t str[5];
  n = 5;
  
  if(value < 0){
    putc('-');
    value = -value;
  }
  uvalue = (uint16_t) value;
  
  do{
    n -=1;
    str[n]=(uvalue % 10) + '0';
    uvalue /= 10;
  } while(uvalue != 0);

  do{
    putc(str[n]);
    n++;
  } while(n<5);
}

///\}

///\}

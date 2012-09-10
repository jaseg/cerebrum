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
* \addtogroup MOD_UART
* \{
**/

/**
* \file
* \brief Internal include for \ref MOD_UART "UART IO".
*	Abstracts register names between MSP430 devices
* \author Alex Mykyta (amykyta3@gmail.com)
**/

///\}

#ifndef __UART_IO_INTERNAL_H__
#define __UART_IO_INTERNAL_H__

//==================================================================================================

#if UIO_CLK_SRC > 2
    #error "Invalid UIO_CLK_SRC in uart_io_config.h"
#endif

//--------------------------------------------------------------------------------------------------
#if UIO_USE_DEV == 0
	#if defined(__MSP430_HAS_USCI_A0__)
		// Registers
		#define UIO_CTL0	UCA0CTL0
		#define UIO_CTL1	UCA0CTL1
		#define UIO_BR0		UCA0BR0
		#define UIO_BR1		UCA0BR1
		#define UIO_MCTL	UCA0MCTL
		#define UIO_STAT	UCA0STAT
		#define UIO_RXBUF	UCA0RXBUF
		#define UIO_TXBUF	UCA0TXBUF
		#define UIO_ABCTL	UCA0ABCTL
		#define UIO_IE		UCA0IE
		#define UIO_IFG		UCA0IFG
		#define UIO_IV		UCA0IV
		
		// ISR
		#define UIO_ISR_SPLIT 0
		#define UIO_ISR_VECTOR	USCI_A0_VECTOR
		
		#define __MSP430_HAS_USCI__
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	#elif defined(UCA0CTL0_) // Older variant of USCI
		// Registers
		#define UIO_CTL0	UCA0CTL0
		#define UIO_CTL1	UCA0CTL1
		#define UIO_BR0		UCA0BR0
		#define UIO_BR1		UCA0BR1
		#define UIO_MCTL	UCA0MCTL
		#define UIO_STAT	UCA0STAT
		#define UIO_RXBUF	UCA0RXBUF
		#define UIO_TXBUF	UCA0TXBUF
		#define UIO_ABCTL	UCA0ABCTL
		
		#define UIO_IE		IE2
		#define UIO_UCATXIE	UCA0TXIE
		#define UIO_UCARXIE	UCA0RXIE
		
		#define UIO_IFG		IFG2
		#define UIO_UCATXIFG	UCA0TXIFG
		#define UIO_UCARXIFG	UCA0RXIFG
		
		// ISR
		#define UIO_ISR_SPLIT 1
		#define UIO_TXISR_VECTOR	USCIAB0TX_VECTOR
		#define UIO_RXISR_VECTOR	USCIAB0RX_VECTOR
		
		#define __MSP430_HAS_UCA__
	#else
		#error "Invalid UIO_USE_DEV in uart_io_config.h"
	#endif
//--------------------------------------------------------------------------------------------------
#elif UIO_USE_DEV == 1
	#if defined(__MSP430_HAS_USCI_A1__)
		// Registers
		#define UIO_CTL0	UCA1CTL0
		#define UIO_CTL1	UCA1CTL1
		#define UIO_BR0		UCA1BR0
		#define UIO_BR1		UCA1BR1
		#define UIO_MCTL	UCA1MCTL
		#define UIO_STAT	UCA1STAT
		#define UIO_RXBUF	UCA1RXBUF
		#define UIO_TXBUF	UCA1TXBUF
		#define UIO_ABCTL	UCA1ABCTL
		#define UIO_IE		UCA1IE
		#define UIO_IFG		UCA1IFG
		#define UIO_IV		UCA1IV
		
		// ISR
		#define UIO_ISR_SPLIT 0
		#define UIO_ISR_VECTOR	USCI_A1_VECTOR
		
		#define __MSP430_HAS_USCI__
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	#elif defined(UCA1CTL0_) // Older variant of USCI
		// Registers
		#define UIO_CTL0	UCA1CTL0
		#define UIO_CTL1	UCA1CTL1
		#define UIO_BR0		UCA1BR0
		#define UIO_BR1		UCA1BR1
		#define UIO_MCTL	UCA1MCTL
		#define UIO_STAT	UCA1STAT
		#define UIO_RXBUF	UCA1RXBUF
		#define UIO_TXBUF	UCA1TXBUF
		#define UIO_ABCTL	UCA1ABCTL
		
		#define UIO_IE		UC1IE
		#define UIO_UCATXIE	UCA1TXIE
		#define UIO_UCARXIE	UCA1RXIE
		
		#define UIO_IFG		UC1IFG
		#define UIO_UCATXIFG	UCA1TXIFG
		#define UIO_UCARXIFG	UCA1RXIFG
		
		// ISR
		#define UIO_ISR_SPLIT 1
		#define UIO_TXISR_VECTOR	USCIAB1TX_VECTOR
		#define UIO_RXISR_VECTOR	USCIAB1RX_VECTOR
		
		#define __MSP430_HAS_UCA__
	#else
		#error "Invalid UIO_USE_DEV in uart_io_config.h"
	#endif
//--------------------------------------------------------------------------------------------------
#elif UIO_USE_DEV == 2
	#if defined(__MSP430_HAS_USCI_A2__)
		// Registers
		#define UIO_CTL0	UCA2CTL0
		#define UIO_CTL1	UCA2CTL1
		#define UIO_BR0		UCA2BR0
		#define UIO_BR1		UCA2BR1
		#define UIO_MCTL	UCA2MCTL
		#define UIO_STAT	UCA2STAT
		#define UIO_RXBUF	UCA2RXBUF
		#define UIO_TXBUF	UCA2TXBUF
		#define UIO_ABCTL	UCA2ABCTL
		#define UIO_IE		UCA2IE
		#define UIO_IFG		UCA2IFG
		#define UIO_IV		UCA2IV
		
		// ISR
		#define UIO_ISR_SPLIT 0
		#define UIO_ISR_VECTOR	USCI_A2_VECTOR
		
		#define __MSP430_HAS_USCI__
	#else
		#error "Invalid UIO_USE_DEV in uart_io_config.h"
	#endif
//--------------------------------------------------------------------------------------------------
#elif UIO_USE_DEV == 3
	#if defined(__MSP430_HAS_USCI_A3__)
		// Registers
		#define UIO_CTL0	UCA3CTL0
		#define UIO_CTL1	UCA3CTL1
		#define UIO_BR0		UCA3BR0
		#define UIO_BR1		UCA3BR1
		#define UIO_MCTL	UCA3MCTL
		#define UIO_STAT	UCA3STAT
		#define UIO_RXBUF	UCA3RXBUF
		#define UIO_TXBUF	UCA3TXBUF
		#define UIO_ABCTL	UCA3ABCTL
		#define UIO_IE		UCA3IE
		#define UIO_IFG		UCA3IFG
		#define UIO_IV		UCA3IV
		
		// ISR
		#define UIO_ISR_SPLIT 0
		#define UIO_ISR_VECTOR	USCI_A3_VECTOR
		
		#define __MSP430_HAS_USCI__
	#else
		#error "Invalid UIO_USE_DEV in uart_io_config.h"
	#endif
//--------------------------------------------------------------------------------------------------
#else
	#error "Invalid UIO_USE_DEV in uart_io_config.h"
#endif

#endif

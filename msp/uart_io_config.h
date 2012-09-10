/**
* \addtogroup MOD_UART
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_UART "UART IO"
* \author Alex Mykyta (amykyta3@gmail.com)
**/

#ifndef __UART_IO_CONFIG_H__
#define __UART_IO_CONFIG_H__

//==================================================================================================
// UART IO Config
//
// Configuration for: PROJECT_NAME
//==================================================================================================

//  ===================================================
//  = NOTE: Actual ports must be configured manually! =
//  ===================================================

/** \addtogroup DEF_UART_CONFIG Configuration Defines
*	\brief Configuration defines for the UART IO module
* \{ **/

///\brief UART IO Operating Mode
#define UIO_USE_INTERRUPTS	1	///< \hideinitializer
/**<	0 = Polling Mode	: No buffers used. RX and TX is done within the function calls. \n
*		1 = Interrupt Mode	: All IO is buffered. RX and TX is handled in interrupts.
**/
	
///\brief RX buffer size (Interrupt mode only)
#define UIO_RXBUF_SIZE	64	///< \hideinitializer

///\brief TX buffer size (Interrupt mode only)
#define UIO_TXBUF_SIZE	64	///< \hideinitializer

/// \brief Select which USCI module to use
#define UIO_USE_DEV		0	///< \hideinitializer
/**<	0 = USCIA0 \n
* 		1 = USCIA1 \n
* 		2 = USCIA2 \n
* 		3 = USCIA3
**/

/// \brief Select which clock source to use
#define UIO_CLK_SRC		2	///< \hideinitializer
/**<	0 = External \n
*		1 = ACLK	\n
*		2 = SMCLK
**/

	/** \addtogroup DEF_UART_BAUDS Baud Rates
	*	\brief Baud rate constants based on the clock source
	* \{ **/
// Baud Rates for SMCLK = 18 MHz
#define UIO_BR0_9600	0x53	///< \hideinitializer
#define UIO_BR1_9600	0x07	///< \hideinitializer
#define UIO_MCTL_9600	0x00	///< \hideinitializer

#define UIO_BR0_19200	0xA9	///< \hideinitializer
#define UIO_BR1_19200	0x03	///< \hideinitializer
#define UIO_MCTL_19200	0x55	///< \hideinitializer

#define UIO_BR0_115200	0x9C	///< \hideinitializer
#define UIO_BR1_115200	0x00	///< \hideinitializer
#define UIO_MCTL_115200	0x22	///< \hideinitializer

// Default Baud Rate
#define UIO_BR0_DEFAULT		UIO_BR0_9600
#define UIO_BR1_DEFAULT		UIO_BR1_9600
#define UIO_MCTL_DEFAULT	UIO_MCTL_9600
	///\}
///\}
	
#endif
///\}

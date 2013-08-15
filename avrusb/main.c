/*
 Copyright (C) 2012 jaseg <s@jaseg.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/power.h>

#include "Descriptors.h"

#include <comm.h>
#include <comm_handle.h>
#include <autocode.h>
#include <config.h>
#include <uart.h>

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Peripheral/Serial.h>

void init(void);
void loop_usb(void);

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
    {
        .Config =
            {
                .ControlInterfaceNumber   = 0,
                .DataINEndpoint           =
                    {
                        .Address          = CDC_TX_EPADDR,
                        .Size             = CDC_TXRX_EPSIZE,
                        .Banks            = 1,
                    },
                .DataOUTEndpoint =
                    {
                        .Address          = CDC_RX_EPADDR,
                        .Size             = CDC_TXRX_EPSIZE,
                        .Banks            = 1,
                    },
                .NotificationEndpoint =
                    {
                        .Address          = CDC_NOTIFICATION_EPADDR,
                        .Size             = CDC_NOTIFICATION_EPSIZE,
                        .Banks            = 1,
                    },
            },
    };

int main(void){
    init();
    for(;;){
		loop_usb();
		loop_auto();
	}
}

void init(){
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
	clock_prescale_set(clock_div_1);
    USB_Init();

	init_auto();

    sei();
}

void loop_usb(){
    int16_t receive_status = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    Endpoint_SelectEndpoint(VirtualSerial_CDC_Interface.Config.DataINEndpoint.Address);

    if(!(receive_status < 0)) {
		uint8_t c = (receive_status & 0xFF);
		comm_handle(c);
	}

	CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	USB_USBTask();
}

void uart_putc(uint8_t c){
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, c);
}

void uart_putc_nonblocking(uint8_t c){
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, c);
}

void EVENT_USB_Device_ConfigurationChanged(void) {
    CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

void EVENT_USB_Device_ControlRequest(void) {
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo) {
} 
 
void EVENT_USB_Device_Connect(void){
}

void EVENT_USB_Device_Disconnect(void){
}



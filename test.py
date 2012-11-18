#!/usr/bin/env python

from pylibcerebrum import pylibcerebrum

g = pylibcerebrum.Ganglion("/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A900F4GW-if00-port0", 115200)
g.ws2801.buffer = tuple( [255, 0, 0] * 512 )

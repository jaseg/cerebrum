
MCU=msp430g2553

all: helights

program: helights
	mspdebug rf2500 "prog helights"

helights: helights.c uart_io.c fifo.c
	msp430-gcc -mmcu=$(MCU) -g -o helights helights.c uart_io.c fifo.c -lm


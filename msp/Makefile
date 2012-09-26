
MCU=msp430g2553

all: helights

program: helights
	mspdebug -d $(DEVICE) rf2500 "prog helights"

helights: helights.c uart_io.c fifo.c comm.c
	msp430-gcc -mmcu=$(MCU) -g -std=gnu99 -o helights helights.c uart_io.c fifo.c comm.c -lm

clean:
	rm helights


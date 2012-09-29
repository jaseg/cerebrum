
MCU=msp430g2553

all: helights

program: helights
	mspdebug -s $(SERIAL) rf2500 "prog helights"

helights: helights.c uart_io.c fifo.c comm.c
	msp430-gcc -mmcu=$(MCU) -g -std=gnu99 -o helights helights.c uart_io.c fifo.c comm.c autocode.c -lm

clean:
	rm helights || true


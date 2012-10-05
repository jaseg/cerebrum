
MCU=msp430g2553

all: main

program: main
	mspdebug -s $(SERIAL) rf2500 "prog main"

main: main.c uart_io.c fifo.c comm.c
	msp430-gcc -mmcu=$(MCU) -g0 -O3 -ffunction-sections -Wl,--gc-sections,--relax -std=gnu99 -Xprint-map -o main main.c uart_io.c fifo.c comm.c autocode.c config.c -lm
	msp430-size main

clean:
	rm main || true


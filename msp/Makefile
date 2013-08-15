
MCU=msp430g2553

all: main

program: main
	mspdebug -s $(SERIAL) rf2500 "prog main"

main: main.c uart.c fifo.c ../common/comm.c
	ssh -o VisualHostKey=no c-leuse rm -rf /tmp/foobarmsp
	ssh -o VisualHostKey=no c-leuse mkdir /tmp/foobarmsp
	rsync -a ./ c-leuse:/tmp/foobarmsp
	scp ../common/comm.c ../common/*.h c-leuse:/tmp/foobarmsp
	ssh -o VisualHostKey=no c-leuse make -C /tmp/foobarmsp remotemain CONFIG_MAC=$(CONFIG_MAC)
	scp c-leuse:/tmp/foobarmsp/main ./
	ssh -o VisualHostKey=no c-leuse rm -rf /tmp/foobarmsp

remotemain: main.c uart.c fifo.c comm.c
	msp430-gcc -mmcu=$(MCU) -g0 -O3 -ffunction-sections -Wl,--gc-sections,--relax -std=gnu99 -Xprint-map -o main main.c uart.c fifo.c comm.c autocode.c config.c -lm -DCONFIG_MAC=$(CONFIG_MAC)
	msp430-size main

clean:
	rm main || true


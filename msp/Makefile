
MCU=msp430g2553

all: helights-$MCU.elf

clean:
	rm -f *.elf

program: helights-$MCU.elf
	sudo mspdebug rf2500 "prog helights-$(MCU).elf"

helights-$MCU.elf: helights.c fifo.c
	msp430-gcc -mmcu=$(MCU) -g -o helights-$(MCU).elf helights.c fifo.c -lm


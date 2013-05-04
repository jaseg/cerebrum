
all: objects


objects: autocode.c config.c uart.c ../common/main.c ../common/comm.c
	avr-gcc -Wall -fshort-enums -fno-inline-small-functions -fpack-struct -Wall -fno-strict-aliasing -funsigned-char -funsigned-bitfields -ffunction-sections -mmcu=${MCU} -DFDEV_SETUP_STREAM -DF_CPU=${CLOCK} -DCEREBRUM_BAUDRATE=${CEREBRUM_BAUDRATE} -DCONFIG_ADDRESS=${CONFIG_ADDRESS} -std=gnu99 -Os -o main.elf -Wl,--gc-sections,--relax -I . -I../common $^
	avr-objcopy -O ihex main.elf main.hex
	avr-size main.elf

program:
	../tools/reset_arduino.py ${PORT}
	avrdude -V -c ${PROGRAMMER} -P ${PORT} -b ${PROGRAMMER_BAUDRATE} -U flash:w:main.hex -p ${MCU}

clean:
	rm main.elf main.hex autocode.h config.c

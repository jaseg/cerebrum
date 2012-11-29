
F_OSC = ${CLOCK}
FORMAT = ihex

TARGET = main
CSRC = main.c uart.c comm.c autocode.c config.c
ASRC =

CFLAGS = -Os -std=gnu99
#CFLAGS += -gdwarf-2
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fms-extensions
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$<.lst
CFLAGS += -DF_OSC=$(F_OSC) -DF_CPU=$(F_OSC)
CFLAGS += -IRF24

CPPFLAGS = -Os
#CPPFLAGS += -gdwarf-2
CPPFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS += -Wall
CPPFLAGS += -Wa,-adhlns=$<.lst
CPPFLAGS += -DF_OSC=$(F_OSC) -DF_CPU=$(F_OSC)
CPPFLAGS += -I. -IRF24

ASFLAGS = -Wa,-adhlns=$<.lst,-gstabs
ASFLAGS += -DF_OSC=$(F_OSC)

LDFLAGS = -Wl,-Map=$(TARGET).map,--cref

OBJ = $(CSRC:.c=.o) $(ASRC:.S=.o)
GENDEPFLAGS = -MD -MP -MF .deppp/$(@F).d
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I. $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

all: build size

build: elf hex eep lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

size:
	@avr-size -A $(TARGET).elf

program: $(TARGET).hex $(TARGET).eep
	@sh -c 'echo>'${PORT}
	@avrdude -qq -p $(MCU) -P ${PORT} -c ${PROGRAMMER} -b ${BAUDRATE} -U flash:w:$(TARGET).hex

%.hex: %.elf
	@avr-objcopy -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@-avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

%.lss: %.elf
	@avr-objdump -h -S $< > $@

%.sym: %.elf
	@avr-nm -n $< > $@

.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)

%.elf: $(OBJ)
	@avr-gcc $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)

%.o : %.c
	@avr-gcc -c $(ALL_CFLAGS) $< -o $@

%.o : %.cpp
	@avr-gcc -c $(ALL_CPPFLAGS) $< -o $@

%.s : %.c
	@avr-gcc -S $(ALL_CFLAGS) $< -o $@

%.s : %.cpp
	@avr-gcc -S $(ALL_CPPFLAGS) $< -o $@

%.o : %.S
	@avr-gcc -c $(ALL_ASFLAGS) $< -o $@

clean:
	@rm -f $(TARGET).hex $(TARGET).eep $(TARGET).obj $(TARGET).elf $(TARGET).map $(TARGET).obj $(TARGET).a90 $(TARGET).sym $(TARGET).lnk $(TARGET).lss
	@rm -f .deppp/*
	@rm -f *.bak  *.BAK *~ *.o *.s *.lst
	@rm -f RF24/*.o RF24/*.s RF24/*.lst
	@rm -f comm-test

-include $(shell mkdir .deppp 2>/dev/null) $(wildcard .deppp/*)

.PHONY : all size build elf hex eep lss sym clean program


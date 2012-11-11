# Hey Emacs, this is a -*- makefile -*-
# Time-stamp: "23.12.10 21:21 Makefile klaus?wachtler.de"
#
#############################################################################
#
# WinAVR makefile written by Eric B. Weddington, Jörg Wunsch, et al.
# Released to the Public Domain
# Please read the make user manual!
#
# Additional material for this makefile was submitted by:
#  Tim Henigan
#  Peter Fleury
#  Reiner Patommel
#  Sander Pool
#  Frederik Rouleau
#  Markus Pfaff
#
# This file version is a proposal to be used for the AVR gcc tutorial
# found at http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial
# (tutorial in german only, sorry).
#
#############################################################################
#
# Usage
# - F_OSC Define in CFLAGS and AFLAGS
#
# klaus?wachtler.de 2009/10/07
# - Adapted for C++ and C files in one project (separate lists for C-, C++- and
#   ASM-Sources, modified names of list files)
#
# klaus?wachtler.de 2010/04/26, see discussion
# on http://www.mikrocontroller.net/topic/175727 (in german)
# - Some new comments
# - defaults to C source again, but may be used for C++
# - settings usually to be customised marked with ==>
#
# klaus@wachtler.de 2010/05/01
# - separate flags for C and C++
#
# Not tested on Windows!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
#
#
#
# (to be countinued...)
#
#############################################################################


# ==> define the name of your project and main source file (without extension)
#
# The gcc tutorial http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial
# uses main.c as the main file, so you have to use the name main here.
# On unix like systems (linux etc.) all file names are case sensitive!
PROJECTNAME=main
# <==

# ==> MCU name
MCU = ${CHIP}
# <==


# ==> Main Oscillator Frequency [Hz]
#
# Note: This setting does NOT change the frequency of the AVR.
#       You have to ensure that the frequency has the following value.
#       This setting is used to trim functions like _delay_ms() etc. to
#       work with your hardware as expected (used in C, C++ and assembler
#       sources).
#       If you are in doubt how to set the frequency of your controller
#       please refer to the corresponding data sheet (www.atmel.com).
#       You have to look for information on something like
#       "memory programming/fuse settings" and "system clock and clock options".
#
F_OSC = 16000000
# <==


# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).
TARGET = $(PROJECTNAME)


# ==> If you have more than one source file then list them here.
#
# The source file named like the project may be given as $(PROJECTNAME).c
# or $(PROJECTNAME).cpp; list the remaining files with their full names
# in CSRC!
# C and C++  dependencies are automatically generated.
#
# example with one C file:
# CSRC = $(PROJECTNAME).c
#
# example with C file and lcd-routines (cut and paste it together with
# lcd-routines.h from mikrocontroller.net):
# CSRC = $(PROJECTNAME).c lcd-routines.c
#
# example with one C++ file:
# CSRC = $(PROJECTNAME).cpp
#
# example with some C++ files (including the main file of the project) and some C files:
# CSRC = $(PROJECTNAME).cpp second.cpp third.cpp a.c another.c

# List C and C++ files here
CSRC = $(PROJECTNAME).c uart.c comm.c autocode.c config.c
# <==


# ==> List Assembler source files here (if any).
#
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC =
# <==



# ==> Optimization level, can be [0, 1, 2, 3, s].
#
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
#
# s will be be a good choice for starting.
# If you want to debug your code then 0 makes your life easier.
OPT = s
# <==


# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUG =                                 # no debug information
#DEBUG = stabs
DEBUG = dwarf-2

# ==> add additional directories for searching include files
#
# List any extra directories to look for include files here.
# Each directory must be separated with a space.
#
EXTRAINCDIRS = . RF24
# <==


# ==> Compiler flag to set the C Standard level.
#
# gnu99 is usually a good choice.
#
# CSTANDARD = -std=c89      # "ANSI" C
# CSTANDARD = -std=gnu89    # c89 plus GCC extensions
# CSTANDARD = -std=c99      # ISO C99 standard (not yet fully implemented)
# CSTANDARD = -std=gnu99    # c99 plus GCC extensions
CSTANDARD = -std=gnu99
# <==


# ==> Place preprocessor definitions here if necessary.
#
# You can define preprocessor values as defined with #define
# in the source.
# A macro may be defined without a value (by naming it in the EXTRA_CDEFS list)
# or with a value following an equal sign (=).
#
# Each value of the list will be appended to a -D option on
# calling the compiler.
#
# Example: if you use my fixpoint routines for C++
# (see http://mfgkw.dyndns.org/fixpoint_release.zip) you will probably
# want to define the values NO_STDSTRING and NO_STDIOSTREAM like:
#EXTRA_CDEFS = NO_STDSTRING NO_STDIOSTREAM
#
#EXTRA_CDEFS = DEBUG=1
#EXTRA_CDEFS = NDEBUG=1
EXTRA_CDEFS =
# <==


# ==> In the same way you can undef certain macros(rarely used):
#
# Each value of the list will be appended to a -U option on
# calling the compile.
#
# With this feature you can switch off predefined preprocessor macros
# and previously defined -D values (see above).
# Don't ask me why you you should define macros above and undefine them
# here.
#
EXTRA_CUNDEFS =
# <==

# ==> Additional options for C only
#
EXTRA_COPTIONS = -Wstrict-prototypes
# <==

# ==> Additional options for C++ only
#
EXTRA_CPPOPTIONS =
# <==


# Compiler flags C
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS =
CFLAGS = -g$(DEBUG)
CFLAGS += $(foreach ICDEF,$(EXTRA_CDEFS),-D"$(ICDEF)")
CFLAGS += $(foreach ICUNDEF,$(EXTRA_CUNDEFS),-U$(ICUNDEF))
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fms-extensions
CFLAGS += -Wall
CFLAGS += -Wa,-adhlns=$<.lst
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += $(EXTRA_COPTIONS)
CFLAGS += -DF_OSC=$(F_OSC)
CFLAGS += -DF_CPU=$(F_OSC)

# Compiler flags C++
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPFLAGS =
CPPFLAGS = -g$(DEBUG)
CPPFLAGS += $(foreach ICDEF,$(EXTRA_CDEFS),-D"$(ICDEF)")
CPPFLAGS += $(foreach ICUNDEF,$(EXTRA_CUNDEFS),-U$(ICUNDEF))
CPPFLAGS += -O$(OPT)
CPPFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS += -Wall
CPPFLAGS += -Wa,-adhlns=$<.lst
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CPPFLAGS += $(EXTRA_CPPOPTIONS)
CPPFLAGS += -DF_OSC=$(F_OSC)
CPPFLAGS += -DF_CPU=$(F_OSC)


# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$<.lst,-gstabs
ASFLAGS += -DF_OSC=$(F_OSC)




# Additional libraries.



# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt


# ==> choose one of the following options, depending on your needs
#
# - no floating point needed in all printf() like calls
# - floating point with non standard simplified printf()
# - floating point with full printf() support
#
# See http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#ga3b98c0d17b35642c0f3e4649092b9f1
#
#PRINTF_LIB =                      # full functionality except but no floating point
PRINTF_LIB = $(PRINTF_LIB_MIN)    # simplified (only # flag regarded), no floating point
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)  # full functionality including floating point (requires MATH_LIB = -lm below)
# <==


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt


# ==> choose one of the following options, depending on your needs
#
# - no floating point needed in all scanf() like calls
# - floating point with non standard simplified scanf() (no %[...] conversion)
# - floating point with full scanf() support
#
# See http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#g67bae1ad3af79809fd770be392f90e21
#
#SCANF_LIB =                    # full functionality except floating point, max. width 255
SCANF_LIB = $(SCANF_LIB_MIN)   # no floating point, no %[, max. width 255
#SCANF_LIB = $(SCANF_LIB_FLOAT) # full, floating point, max. width 65535 (requires MATH_LIB = -lm below)
# <==


# ==> choose one of the following
#
# If your code contains math functions or floating point, then use the -lm version.
# If not, don't use it to save program space.
#
# See options PRINTF_LIB and SCANF_LIB above!
#
# Note: if your code size grows too high then consider using fixed point
# arithmetic instead of floating point arithmetic.
# You may expect smaller and faster programs.
#
MATH_LIB =
#MATH_LIB = -lm
# <==


# ==> If your controller uses external memory:

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff
#
EXTMEMOPTS =
# <==



# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)



# ==> Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = ${PROGRAMMER} -b ${BAUDRATE}
# <==


# ==> Choose the port used by the programmer
AVRDUDE_PORT = ${PORT}
# <==



AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)
#AVRDUDE_FLAGS += -E noreset



# ---------------------------------------------------------------------------

# Define directories, if needed.
DIRAVR = c:/winavr
DIRAVRBIN = $(DIRAVR)/bin
DIRAVRUTILS = $(DIRAVR)/utils/bin
DIRINC = .
DIRLIB = $(DIRAVR)/avr/lib


# Define programs and commands.
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp




# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:




# Define all object files.
OBJ = $(CSRC:.c=.o) $(ASRC:.S=.o)

# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .deppp/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I. $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.

all: begin gccversion sizebefore build sizeafter finished end

build: elf hex eep lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym



# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
sizebefore:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi



# Display compiler version information.
gccversion :
	@$(CC) --version


# Program the device.
program: $(TARGET).hex $(TARGET).eep
	sh -c 'echo>'$(AVRDUDE_PORT)
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof



# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@



# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create object files from C++ source files.
%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CPPFLAGS) $< -o $@


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C++ source files.
%.s : %.cpp
	$(CC) -S $(ALL_CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean: begin clean_list finished end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) .deppp/*
	$(REMOVE) *.bak  *.BAK *~ *.o *.s *.lst
	$(REMOVE) RF24/*.o RF24/*.s RF24/*.lst



# Include the dependency files.
-include $(shell mkdir .deppp 2>/dev/null) $(wildcard .deppp/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program


## target mcu; avr-gcc and avrdude usually have different mcu-names
MCU	= attiny13
DUDEMCU	= t13

## avrdude options
DUDEPROGRAMMER	= dragon_isp
DUDEPORT	= usb

## compiler definitions
CC	= avr-gcc
OBJCOPY	= avr-objcopy
OBJDUMP = avr-objdump

SRCDIR=src
AVRCRYPTOLIBDIR=libs/avrcryptolib

CFLAGS=-mmcu=$(MCU) -Wall -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -I$(SRCDIR) -I$(AVRCRYPTOLIBDIR)
ASMFLAGS=-mmcu=$(MCU) -Wall -I$(SRCDIR) -I$(AVRCRYPTOLIBDIR) -assembler-with-cpp
LDFLAGS=-mmcu=$(MCU) -Wl,-Map=Map.map -L$(SRCDIR) -L$(AVRCRYPTOLIBDIR)
DEBUGFLAGS=-g -gdwarf-2

## compilation rules

stratumkey.hex: stratumkey.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey.elf: src/main.o libs/avrcryptolib/sha256.S.o
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@
	mv Map.map stratumkey.map

libs/avrcryptolib/sha256.S.o: libs/avrcryptolib/sha256-asm.S
	$(CC) $(ASMFLAGS) -o $@ -c $<

src/main.o: src/main.c
	$(CC) $(CFLAGS) -o $@ -c $<

#%.eep: %.elf
#	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0
#
#%.lss: %.elf
#	$(OBJDUMP) -h -S $< > $@

upload: stratumkey.hex
	avrdude -c $(DUDEPROGRAMMER) -P $(DUDEPORT) -p $(DUDEMCU) -U flash:w:$<:i
	rm -f *.map

#.PHONY: %.all
#%.all:
#	$(CC) $(CFLAGS) $(DEBUGFLAGS) -MD -MP -MT $*.o -MF $*.o.d -c $*.c
#	$(CC) $(LDFLAGS) $*.o -o $*.elf
#	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $*.elf $*.hex
#	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $*.elf $*.eep || exit 0
#	$(OBJDUMP) -h -S $*.elf > $*.lss

cleanall: clean cleanhex cleandep

cleanhex:
	rm -f *.hex

cleandep:
	rm -f *.o.d

clean:
	rm -f *.o *.map *.elf *.eep *.lss
	rm -f libs/avrcryptolib/sha256.S.o
	rm -f src/main.o

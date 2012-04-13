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
OWDIR=libs/1wire
CRYPTDIR=libs/avrcryptolib

CFLAGS=-mmcu=$(MCU) -Wall -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -fno-exceptions -I$(SRCDIR) -I$(OWDIR) -I$(CRYPTDIR)
ASMFLAGS=-mmcu=$(MCU) -Wall -I$(SRCDIR) -I$(OWDIR) -I$(CRYPTDIR) -assembler-with-cpp
LDFLAGS=-mmcu=$(MCU) -Wl,-Map=Map.map -Wl,-gc-sections
DEBUGFLAGS=-g -gdwarf-2

## compilation rules

stratumkey_slave.hex: stratumkey_slave.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey_slave.elf: main_slave.o 1wire.o sha256.S.o
	$(CC) $(LDFLAGS) $^ -o $@
	mv Map.map stratumkey_slave.map

sha256.S.o: libs/avrcryptolib/sha256-asm.S
	$(CC) $(ASMFLAGS) -o $@ -c $<

1wire.o: libs/1wire/1wire.c
	$(CC) $(CFLAGS) -o $@ -c $<

main_slave.o: src/main_slave.c
	$(CC) $(CFLAGS) -o $@ -c $<

#%.eep: %.elf
#	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0
#
#%.lss: %.elf
#	$(OBJDUMP) -h -S $< > $@

upload_slave: stratumkey_slave.hex
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

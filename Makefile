## target mcu; avr-gcc and avrdude usually have different mcu-names
# attiny13, attiny85, atmega8
MCU_SLAVE	= atmega8
# t13, t85, m8
DUDEMCU_SLAVE	= m8
F_CPU_SLAVE = 8000000UL
MCU_MASTER	= atmega8
DUDEMCU_MASTER	= m8
F_CPU_MASTER = 8000000UL

## avrdude options
DUDEPROGRAMMER	= dragon_isp
DUDEPORT	= usb

## compiler definitions
CC	= avr-gcc
OBJCOPY	= avr-objcopy
OBJDUMP = avr-objdump

SRCDIR=src
LIBDIR=libs

CFLAGS=-Wall -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -fno-exceptions -I$(SRCDIR) -I$(LIBDIR)
ASMFLAGS=-Wall -I$(SRCDIR) -I$(LIBDIR) -assembler-with-cpp
LDFLAGS=-Wl,-Map=Map.map -Wl,-gc-sections
DEBUGFLAGS=-g -gdwarf-2

## compilation rules
## master
stratumkey_master.hex: stratumkey_master.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey_master.elf: main_master.o sha256_master.S.o single_wire_UART_master.o
	$(CC) -mmcu=$(MCU_MASTER) $(LDFLAGS) $^ -o $@
	mv Map.map stratumkey_master.map

sha256_master.S.o: libs/avrcryptolib/sha256-asm.S
	$(CC) -mmcu=$(MCU_MASTER) $(ASMFLAGS) -o $@ -c $<

single_wire_UART_master.o: libs/single_wire_uart/single_wire_UART.c
	$(CC) -mmcu=$(MCU_MASTER) -DF_CPU=$(F_CPU_MASTER) -D$(MCU_MASTER) $(CFLAGS) -o $@ -c $<

main_master.o: src/main_master.c
	$(CC) -mmcu=$(MCU_MASTER) -DF_CPU=$(F_CPU_MASTER) -D$(MCU_MASTER) $(CFLAGS) -o $@ -c $<

upload_master: stratumkey_master.hex
	avrdude -c $(DUDEPROGRAMMER) -P $(DUDEPORT) -p $(DUDEMCU_MASTER) -U flash:w:$<:i
	rm -f *.map

## slave
stratumkey_slave.hex: stratumkey_slave.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey_slave.elf: main_slave.o sha256_slave.S.o single_wire_UART_slave.o
	$(CC) -mmcu=$(MCU_SLAVE) $(LDFLAGS) $^ -o $@
	mv Map.map stratumkey_slave.map

sha256_slave.S.o: libs/avrcryptolib/sha256-asm.S
	$(CC) -mmcu=$(MCU_SLAVE) $(ASMFLAGS) -o $@ -c $<

single_wire_UART_slave.o: libs/single_wire_uart/single_wire_UART.c
	$(CC) -mmcu=$(MCU_SLAVE) -DF_CPU=$(F_CPU_SLAVE) -D$(MCU_SLAVE) $(CFLAGS) -o $@ -c $<

main_slave.o: src/main_slave.c
	$(CC) -mmcu=$(MCU_SLAVE) -DF_CPU=$(F_CPU_SLAVE) -D$(MCU_SLAVE) $(CFLAGS) -o $@ -c $<

upload_slave: stratumkey_slave.hex
	avrdude -c $(DUDEPROGRAMMER) -P $(DUDEPORT) -p $(DUDEMCU_SLAVE) -U flash:w:$<:i
	rm -f *.map

#%.eep: %.elf
#	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0
#
#%.lss: %.elf
#	$(OBJDUMP) -h -S $< > $@

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

## target mcu; avr-gcc and avrdude usually have different mcu-names
# attiny13, attiny85, atmega8
MCU	= atmega8
# t13, t85, m8
DUDEMCU	= m8
F_CPU = 8000000UL

## avrdude options
DUDEPROGRAMMER	= dragon_isp
DUDEPORT	= usb

## compiler definitions
CC	= avr-gcc
OBJCOPY	= avr-objcopy
OBJDUMP = avr-objdump

SRCDIR=../src
LIBDIR=../libs

CFLAGS=-Wall -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -fno-exceptions -I$(SRCDIR) -I$(LIBDIR)
ASMFLAGS=-Wall -I$(SRCDIR) -I$(LIBDIR) -assembler-with-cpp
LDFLAGS=-Wl,-Map=Map.map -Wl,-gc-sections
DEBUGFLAGS=-g -gdwarf-2

## compilation rules
## master
stratumkey_master.hex: stratumkey_master.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey_master.elf: main_master.o sha256.S.o uart_io.o swu_highlevel.o single_wire_UART.o
	$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ -o $@
	mv Map.map stratumkey_master.map

sha256.S.o: $(LIBDIR)/avrcryptolib/sha256-asm.S
	$(CC) -mmcu=$(MCU) $(ASMFLAGS) -o $@ -c $<

uart_io.o: $(LIBDIR)/uart_io/uart_io.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -D$(MCU) $(CFLAGS) -o $@ -c $<

swu_highlevel.o: $(LIBDIR)/single_wire_uart/swu_highlevel.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -D$(MCU) $(CFLAGS) -o $@ -c $<
  
single_wire_UART.o: $(LIBDIR)/single_wire_uart/single_wire_UART.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -D$(MCU) $(CFLAGS) -o $@ -c $<

main_master.o: $(SRCDIR)/main_master.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -D$(MCU) $(CFLAGS) -o $@ -c $<

upload_master: stratumkey_master.hex
	avrdude -c $(DUDEPROGRAMMER) -P $(DUDEPORT) -p $(DUDEMCU) -U flash:w:$<:i
	rm -f *.map

cleanall: clean cleanhex cleandep

cleanhex:
	rm -f *.hex

cleandep:
	rm -f *.o.d

clean:
	rm -f *.o *.map *.elf *.eep *.lss

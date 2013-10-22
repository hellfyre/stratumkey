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

SRCDIR=src
LIBDIR=../libs

CFLAGS=-Wall -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -fno-exceptions -I$(SRCDIR) -I$(LIBDIR) -DSTRATUMKEY_UART
ASMFLAGS=-Wall -I$(SRCDIR) -I$(LIBDIR)
LDFLAGS=-Wl,-Map=Map.map -Wl,-gc-sections
DEBUGFLAGS=-g -gdwarf-2

## compilation rules
## master
.PHONY: all
all: stratumkey_master.hex

stratumkey_master.hex: stratumkey_master.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey_master.elf: main.o sha256.S.o uart_io.o swu_highlevel.o single_wire_UART.o serial_message.o
	$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ -o $@
	mv Map.map stratumkey_master.map

sha256.S.o: $(LIBDIR)/avrcryptolib/sha256-asm.S
	$(CC) -mmcu=$(MCU) $(ASMFLAGS) -o $@ -c $<

uart_io.o: $(LIBDIR)/uart_io/uart_io.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(CFLAGS) -o $@ -c $<

swu_highlevel.o: $(LIBDIR)/single_wire_uart/swu_highlevel.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -include $(LIBDIR)/single_wire_uart/swu_device_defines_$(MCU).h $(CFLAGS) -o $@ -c $<
  
single_wire_UART.o: $(LIBDIR)/single_wire_uart/single_wire_UART.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -include $(LIBDIR)/single_wire_uart/swu_device_defines_$(MCU).h $(CFLAGS) -o $@ -c $<

serial_message.o: $(LIBDIR)/serial_message/serial_message.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -include $(LIBDIR)/single_wire_uart/swu_device_defines_$(MCU).h $(CFLAGS) -o $@ -c $<

main.o: $(SRCDIR)/main.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -include $(LIBDIR)/single_wire_uart/swu_device_defines_$(MCU).h $(CFLAGS) -o $@ -c $<

.PHONY: upload
upload: stratumkey_master.hex
	avrdude -c $(DUDEPROGRAMMER) -P $(DUDEPORT) -p $(DUDEMCU) -U flash:w:$<:i
	rm -f *.map

.PHONY: cleanall
cleanall: clean cleanhex cleandep

.PHONY: cleanhex
cleanhex:
	rm -f *.hex

.PHONY: cleandep
cleandep:
	rm -f *.o.d

.PHONY: clean
clean:
	rm -f *.o *.map *.elf *.eep *.lss

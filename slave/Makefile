## target mcu; avr-gcc and avrdude usually have different mcu-names
# attiny13, attiny85, atmega8
MCU	= attiny85
F_CPU = 8000000UL

## avrdude options
PROGRAMMER	= dragon_isp
PORT	= usb

## compiler definitions
CC	= avr-gcc
OBJCOPY	= avr-objcopy
OBJDUMP = avr-objdump

SRCDIR=src
LIBDIR=../libs
DEVDEFDIR=../device_defines

CFLAGS=-Wall -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -fno-exceptions -I$(SRCDIR) -I$(LIBDIR) -I$(DEVDEFDIR)
ASMFLAGS=-Wall -I$(SRCDIR) -I$(LIBDIR)
LDFLAGS=-Wl,-Map=Map.map -Wl,-gc-sections
DEBUGFLAGS=-g -gdwarf-2

## compilation rules
## slave
.PHONY: all
all: stratumkey_slave.hex

stratumkey_slave.hex: stratumkey_slave.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

stratumkey_slave.elf: main.o sha256.S.o swu_highlevel.o single_wire_UART.o eeprom_io.o serial_message.o
	$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ -o $@
	mv Map.map stratumkey_slave.map

sha256.S.o: $(LIBDIR)/avrcryptolib/sha256-asm.S
	$(CC) -mmcu=$(MCU) $(ASMFLAGS) -o $@ -c $<

swu_highlevel.o: $(LIBDIR)/single_wire_uart/swu_highlevel.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DMCU_$(MCU) $(CFLAGS) -o $@ -c $<
  
single_wire_UART.o: $(LIBDIR)/single_wire_uart/single_wire_UART.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DMCU_$(MCU) $(CFLAGS) -o $@ -c $<

eeprom_io.o: $(LIBDIR)/eeprom_io/eeprom_io.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(CFLAGS) -o $@ -c $<

serial_message.o: $(LIBDIR)/serial_message/serial_message.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DMCU_$(MCU) $(CFLAGS) -o $@ -c $<

main.o: $(SRCDIR)/main.c
	$(CC) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DMCU_$(MCU) $(CFLAGS) -o $@ -c $<

.PHONY: upload
upload: stratumkey_slave.hex
	avrdude -c $(PROGRAMMER) -P $(PORT) -p $(MCU) -U flash:w:$<:i
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

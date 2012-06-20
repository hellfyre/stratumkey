#include <avr/io.h>
#include <avr/eeprom.h>

uint8_t EEPROM_read(uint8_t *addr) {
  eeprom_busy_wait();
  return eeprom_read_byte(addr);
}

void EEPROM_write(uint8_t addr, uint8_t data) {
  eeprom_busy_wait();

  EECR = 0; // Set programming mode. Everything else is zero anyway.

  EEARL = addr;
  EEDR  = data;

  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
}

void eeprom_load_id(uint16_t *id) {
  *id = 0;
  *id ^= EEPROM_read(0);
  *id <<= 8;
  *id ^= EEPROM_read(1);
}

void eeprom_load_secret(uint8_t *secret) {
}

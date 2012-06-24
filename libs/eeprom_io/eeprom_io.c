#include <avr/io.h>
#include <avr/eeprom.h>

uint8_t EEPROM_read(uint8_t *addr) {
  eeprom_busy_wait();
  return eeprom_read_byte(addr);
}

void eeprom_load_id(uint8_t *id) {
  id[0] = EEPROM_read((uint8_t*)0);
  id[1] = EEPROM_read((uint8_t*)1);
}

void eeprom_load_secret(uint8_t *secret) {
  for (int i=0; i<32; i++) {
    secret[i] = EEPROM_read((uint8_t*)i+2);
  }
}

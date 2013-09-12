#include <avr/io.h>
#include <avr/eeprom.h>

uint8_t EEPROM_read(uint8_t *addr) {
  eeprom_busy_wait();
  return eeprom_read_byte(addr);
}

void eeprom_load(uint8_t *data, uint8_t start, uint8_t len) {
  for (int i=start; i<len+start; i++) {
    data[i] = EEPROM_read((uint8_t*)i);
  }
}

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"
#include "eeprom_io/eeprom_io.h"

uint8_t challenge[32];
uint8_t id[2];
uint8_t secret[32];
sha256_hash_t hash;

int main(void) {
  eeprom_load_id(id);
  eeprom_load_secret(secret);

  sei();
  SW_UART_Enable();

  /*----- Send start condition -----*/
  SW_UART_Transmit(0x99);
  /*----- Transmit ID -----*/
  swu_transmit(id, 2);

  WDTCR |= _BV(WDP0);
  WDTCR |= _BV(WDE);
  
  /*----- Receive challenge -----*/
  swu_receive(challenge, 32);

  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;

  /*----- AND challenge and secret and hash the result -----*/
  for (int i=0; i<32; i++) {
    challenge[i] &= secret[i];
  }
  sha256(&hash, challenge, 256);

  /*----- Transmit response -----*/
  swu_transmit(hash, 32);
}

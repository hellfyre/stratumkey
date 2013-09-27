#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"
#include "eeprom_io/eeprom_io.h"

uint8_t key1[32];
uint8_t key2[32];
uint8_t uuid[16];
sha256_hash_t hash;

int main(void) {
  eeprom_load(key1,  0, 32);
  eeprom_load(key2, 32, 32);
  eeprom_load(uuid, 64, 16);

  sei();
  SW_UART_Enable();

  /*----- Transmit ID + key2 -----*/
  uint8_t first_msg[50];

  first_msg[0] = 0xa1;
  memcpy(first_msg+1, uuid, 16);
  memcpy(first_msg+17, key2, 32);
  first_msg[49] = '\n';

  swu_transmit(first_msg, 50);

  // Enable watchdog
  WDTCR |= _BV(WDP0);
  WDTCR |= _BV(WDE);
  
  /*----- Receive challenge -----*/
  uint8_t second_msg[34];
  uint8_t challenge[32];

  swu_receive(second_msg, 34);
  if (second_msg[0] == 0xa2) {
    memcpy(challenge, second_msg+1, 32);
  }

  // Disable watchdog
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;

  /*----- Concatenate challenge and key1 and hash the result -----*/
  uint8_t chall_key1[64];

  memcpy(chall_key1, challenge, 32);
  memcpy(chall_key1+32, key1, 32);
  sha256(&hash, chall_key1, 512);

  /*----- Transmit response -----*/
  uint8_t response[34];
  
  response[0] = 0xa3;
  memcpy(response+1, hash, 32);
  response[33] = '\n';
  swu_transmit(response, 34);
}

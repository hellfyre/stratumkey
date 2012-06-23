#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"

#include "secret.h"

uint8_t challenge[32];
uint8_t id[2];
uint8_t *secret;
sha256_hash_t hash;

int main(void) {
  SETSECRET
  secret = secret_v;
  id[0] = 0;
  id[1] = 13;

  sei();
  SW_UART_Enable();

  /*----- Send start condition -----*/
  SW_UART_Transmit(0x99);
  /*----- Transmit ID -----*/
  swu_transmit(id, 2);
  
  /*----- Receive challenge -----*/
  swu_receive(challenge, 32);

  /*----- AND challenge and secret and hash the result -----*/
  for (int i=0; i<32; i++) {
    challenge[i] &= secret[i];
  }
  sha256(&hash, challenge, 256);

  /*----- Transmit response -----*/
  swu_transmit(hash, 32);

}

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"

#include "secret.h"

void blink(uint8_t times);

uint8_t challenge[32];
uint8_t response[32];
sha256_hash_t hash;
uint8_t *secrets[32];

int main(void) {
  SETSECRET
  uint8_t buffer = 0x00;

  secrets[13] = secret_v;

  sei();
  SW_UART_Enable();

  /*----- Outer loop -----*/
  while(1) {

    /*----- Wait for start condition -----*/
    while(buffer != 0x99) {
      while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}
      buffer = SW_UART_Receive();
    }
    buffer = 0x00;

    /*----- Generate random challenge -----*/
    for (int i=0; i<8; i++) {
      uint32_t random_single = random();
      for (int j=(i*4); j<(i+1)*4; j++) {
        challenge[j] = random_single & 0xff;
        random_single >>= 8;
      }
    }

    /*----- Transmit challenge -----*/
    swu_transmit(challenge, 32);
    /*----- DEBUG: Wait one second (slave blinks debug msg) -----*/
    //_delay_ms(1000);


    /*----- Receive ID -----*/
    uint16_t id = 0;
    id = swu_receive_id();
    /*----- Sanity check -----*/
    if (id > 0xffff) continue;
    if (secrets[id] == NULL) continue;
    if (id != 13) blink(5);

    /*----- Receive response -----*/
    swu_receive(response, 32);

    /*----- AND challenge and secret and hash the result -----*/
    for (int i=0; i<32; i++) {
      challenge[i] &= secrets[id][i];
    }
    sha256(&hash, challenge, 256);

    /*----- Check response -----*/
    int response_correct = 0;
    for (int i=0; i<32; i++) {
      if (response[i] == hash[i]) response_correct++;
    }
    if (response_correct == 32) blink(3);
    else blink(1);
  }
}

void blink(uint8_t times) {
  for (int i=0; i<times; i++) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(500);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(500);
  }
  _delay_ms(500);
}

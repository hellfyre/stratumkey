#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "main_master.h"
#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"

#include "secret.h"

uint8_t challenge[32];
uint8_t response[32];
sha256_hash_t hash;
uint8_t *secrets[32];
uint16_t id;

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

    /*----- DEBUG: Confirm start condition -----*/
    blink(2);

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
    _delay_ms(1000);


    /*----- Receive ID -----*/
    id = 0;
    for (int i=1; i>=0; i--) {
      while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}
      id ^= SW_UART_Receive();
      id <<= i*8;
    }
    /*----- Sanity check -----*/
    if (id > 0xffff) continue;
    if (secrets[id] == NULL) continue;
    if (id != 13) blink(5);

    /*----- Receive response -----*/
    for (int i=0; i<32; i++) {
      while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}
      response[i] = SW_UART_Receive();
    }

    /*----- DEBUG: Confirm reception of response -----*/
    blink(1);

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

void peak() {
  DDRB = _BV(PB6);
  PORTB = _BV(PB6);
  _delay_us(1);
  DDRB = 0;
  PORTB = 0;
}

void morse_byte(uint8_t data) {
  for (int i=0; i<8; i++) {
    uint8_t temp = data & 0x01;
    if (temp) {
      DDRB = _BV(PB6);
      PORTB = _BV(PB6);
      _delay_us(50);
      DDRB = 0;
      PORTB = 0;
      _delay_us(10);
    }
    else {
      DDRB = _BV(PB6);
      PORTB = _BV(PB6);
      _delay_us(10);
      DDRB = 0;
      PORTB = 0;
      _delay_us(50);
    }
    data >>= 1;
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

void blink_long(uint8_t times) {
  for (int i=0; i<times; i++) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(2000);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(2000);
  }
  _delay_ms(2000);
}

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "main_master.h"
#include "single_wire_uart/single_wire_UART.h"
#include "avrcryptolib/sha256.h"

uint8_t secret[32];
uint8_t challenge[32];
uint8_t response[32];

int main(void) {
  uint8_t buffer;

  sei();
  SW_UART_Enable();

  while(1) {
    while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}

    buffer = SW_UART_Receive();
    /*
    if (buffer != 0x99) {
      continue;
    }
    */

    int i, j;
    for (i=0; i<8; i++) {
      uint32_t random_single = random();
      for (j=(i*4); j<(i+1)*4; j++) {
        challenge[j] = random_single & 0xff;
        random_single >>= 8;
      }
    }
    challenge[0] = 0xaa;
    challenge[1] = 0xbb;
    challenge[2] = 0xcc;
    challenge[3] = 0xdd;

    for (i=0; i<32; i++) {
      SW_UART_Transmit(challenge[i]);
    }
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
  int i;
  for (i=0; i<8; i++) {
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
  int i;
  for (i=0; i<times; i++) {
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
  int i;
  for (i=0; i<times; i++) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(2000);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(2000);
  }
  _delay_ms(2000);
}

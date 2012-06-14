#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "main_slave.h"
#include "single_wire_uart/single_wire_UART.h"
#include "avrcryptolib/sha256.h"

#include "secret.h"

uint8_t challenge[32];
uint8_t response[32];

int main(void) {
  SETSECRET
  uint8_t buffer;

  sei();
  SW_UART_Enable();

  /*----- Send start condition -----*/
  SW_UART_Transmit(0x99);
  
  int i;
  /*----- Receive challenge -----*/
  for (i=0; i<32; i++) {
    while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}
    challenge[i] = SW_UART_Receive();
  }

  /*----- DEBUG: Confirm reception of challenge -----*/
  blink(1);

  /*----- AND challenge and secret and hash the result -----*/
  for (i=0; i<32; i++) {
    challenge[i] &= secret[i];
  }
  sha256(response, challenge, 256);

  /*----- Transmit response -----*/
  for (i=0; i<32; i++) {
    SW_UART_Transmit(response[i]);
    _delay_ms(2);
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

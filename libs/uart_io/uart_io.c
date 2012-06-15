#include <avr/io.h>

#include <stdint.h>

void uart_init() {
  // 8n1
  UCSRB |= _BV(TXEN) | _BV(RXEN);
  UCSRC |= _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

  // baudrate 250k @ 8MHz
  UBRRH = 0;
  UBRRL = 51;
}

void uart_receive(uint8_t *data, uint8_t length) {
  for (int i=0; i<length; i++) {
    while ( !(UCSRA & (1<<RXC)) ) {}
    data[i] = UDR;
  }
}

void uart_transmit(uint8_t *data, uint8_t length) {
  for (int i=0; i<length; i++) {
    while ( !( UCSRA & (1<<UDRE)) ) {} // wait for empty transmit buffer
    UDR = data[i];
  }
}

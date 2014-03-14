#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#include "uart_io.h"

void uart_init() {

  // 8n1
  STATUS_B |= _BV(TRANSMITTER_EN) | _BV(RECEIVER_EN);
  //UCSRC |= _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

  // Receive complete interrupt enable
  STATUS_B |= _BV(RX_INT_EN);

  // baudrate 9600 @ 8MHz
  BAUD_HIGH = 0;
  BAUD_LOW = 51;
}

void uart_receive(uint8_t *data, uint8_t length) {
  for (int i=0; i<length; i++) {
    while ( !(STATUS_A & (1<<RECV_COMPLETE)) ) {}
    data[i] = DATA_REG;
  }
}

void uart_transmit(uint8_t *data, uint8_t length) {
  for (int i=0; i<length; i++) {
    while ( !( STATUS_A & (1<<DATA_REG_EMPT)) ) {} // wait for empty transmit buffer
    DATA_REG = data[i];
  }
}

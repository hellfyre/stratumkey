#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#include "uart_io.h"

/* Callback functions */
uart_datarecv_cb_t uart_datarecv_callback = NULL;

// Register callback. Overwrite previously registered callback.
void uart_datarecv_cb_register(uart_datarecv_cb_t cb)
{
  uart_datarecv_callback = cb;
}

void uart_datarecv_cb_unregister() {
  uart_datarecv_callback = NULL;
}

void uart_datarecv_cb_dispatch(uint8_t data)
{
    uart_datarecv_callback(data);
}

ISR(USART_RXC_vect) {
  if (uart_datarecv_callback != NULL) {
    uart_datarecv_cb_dispatch(UDR);
  }
}

void uart_init() {
  // 8n1
  UCSRB |= _BV(TXEN) | _BV(RXEN);
  UCSRC |= _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

  // Receive complete interrupt enable
  UCSRB |= _BV(RXCIE);

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

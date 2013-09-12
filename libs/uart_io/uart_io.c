#include <avr/io.h>
#include <stdlib.h>
#include <string.h>

#include "uart_io.h"

/* Callback functions */
uart_datarecv_cb_t uart_datarecv_callback = 0;
uint8_t uart_receive_buffer[UART_CB_RECV_BUFFER_SIZE];
uint8_t uart_receive_buffer_ctr = 0;

void uart_datarecv_accumulate(uint8_t data)
{
  if (uart_receive_buffer_ctr == UART_CB_RECV_BUFFER_SIZE) {
    // there is no message as long as the buffer, something went wrong
    memset(uart_receive_buffer, 0, UART_CB_RECV_BUFFER_SIZE);
    uart_receive_buffer_ctr = 0;
  }

  uart_receive_buffer[uart_receive_buffer_ctr++] = data;
  if (data == '\n') { // end of message indicated by '\n'
    uart_datarecv_cb_dispatch();
    memset(uart_receive_buffer, 0, UART_CB_RECV_BUFFER_SIZE);
    uart_receive_buffer_ctr = 0;
  }
}

// Register callback. Overwrite previously registered callback.
void uart_datarecv_cb_register(uart_datarecv_cb_t cb)
{
  uart_datarecv_callback = cb;
}

void uart_datarecv_cb_dispatch()
{
  if (uart_datarecv_callback != 0) {
    uint8_t *data = malloc(uart_receive_buffer_ctr);
    memcpy(data, uart_receive_buffer, uart_receive_buffer_ctr);
    uart_datarecv_callback(data, uart_receive_buffer_ctr);
    free(data);
  }
}

SIGNAL(SIG_UART_RECV) {
  uart_datarecv_accumulate(UDR);
}

void uart_init() {
  memset(uart_receive_buffer, 0, UART_CB_RECV_BUFFER_SIZE);

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

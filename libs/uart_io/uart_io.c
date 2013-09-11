#include <avr/io.h>

#include <stdint.h>

/* Callback functions */
extern uart_datarecv_cb_t uart_datarecv_callback = 0;
extern uint8_t uart_receive_buffer[CB_RECV_BUFFER_SIZE];
extern uint8_t uart_receive_buffer_ctr = 0;

void uart_datarecv_accumulate(uint8_t data)
{
  if (data == '\n' || uart_receive_buffer_ctr == CB_RECV_BUFFER_SIZE) {
    uart_datarecv_cb_dispatch(uart_receive_buffer);
    for (int i=0; i<CB_RECV_BUFFER_SIZE; i++) {
      uart_receive_buffer[i] = 0;
    }
    uart_receive_buffer_ctr = 0;
  }
  else {
    uart_receive_buffer[uart_receive_buffer_ctr++] = data;
  }
}

// Register callback. Overwrite previously registered callback.
void uart_datarecv_cb_register(uart_datarecv_cb_t cb)
{
  uart_datarecv_callback = cb;
}

void uart_datarecv_cb_dispatch(uint8_t *data)
{
  uart_datarecv_callback(data);
}

SIGNAL(SIG_UART_RECV) {
  uart_datarecv_accumulate(UDR);
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

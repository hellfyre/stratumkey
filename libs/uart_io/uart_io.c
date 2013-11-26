#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#include "uart_io.h"

#define RINGBUFFER_IDLE       0
#define RINGBUFFER_FIRST_BYTE 2
#define RINGBUFFER_RECVD_SIZE 4
#define ringbuffer_size 64

/* Callback functions */
uart_datarecv_cb_t uart_datarecv_callback = NULL;
volatile uint8_t ringbuffer_status;
volatile uint8_t ringbuffer_bytes_to_read;
volatile uint8_t ringbuffer[ringbuffer_size];
volatile uint8_t ringbuffer_writeptr;
volatile uint8_t ringbuffer_readptr;

// Register callback. Overwrite previously registered callback.
void uart_datarecv_cb_register(uart_datarecv_cb_t cb) {
  uart_datarecv_callback = cb;
}

void uart_datarecv_cb_unregister() {
  uart_datarecv_callback = NULL;
}

ISR(UART_ISR) {
  uart_ringbuffer_write(DATA_REG);
}

uint8_t uart_ringbuffer_read(uint8_t *data, uint8_t len) {
  if (ringbuffer_readptr == ringbuffer_writeptr) {
    return 2;
  }

  for (uint8_t i=0; i<len; i++) {
    data[i] = ringbuffer[ringbuffer_readptr];
    ringbuffer[ringbuffer_readptr] = 0;
    ringbuffer_readptr++;
    if (ringbuffer_readptr == ringbuffer_size) {
      ringbuffer_readptr = 0;
    }
  }

  if (ringbuffer_readptr == ringbuffer_writeptr) {
    return 1;
  }

  return 0;
}

void uart_ringbuffer_write(uint8_t data) {

  ringbuffer[ringbuffer_writeptr++] = data;
  if (ringbuffer_writeptr == ringbuffer_size) {
    ringbuffer_writeptr = 0;
  }

  if (ringbuffer_status == RINGBUFFER_IDLE) {
    ringbuffer_status = RINGBUFFER_FIRST_BYTE;
  }
  else if (ringbuffer_status == RINGBUFFER_FIRST_BYTE) {
    ringbuffer_status = RINGBUFFER_RECVD_SIZE;
    ringbuffer_bytes_to_read = data;
  }
  else if (ringbuffer_status == RINGBUFFER_RECVD_SIZE) {
    ringbuffer_bytes_to_read--;
    if (ringbuffer_bytes_to_read == 0) {
      ringbuffer_status = RINGBUFFER_IDLE;
      if (uart_datarecv_callback) {
        uart_datarecv_callback();
      }
    }
  }
}

void uart_init() {

  ringbuffer_status = RINGBUFFER_IDLE;
  ringbuffer_bytes_to_read = 0;
  memset(ringbuffer, 0, ringbuffer_size);
  ringbuffer_writeptr = 0;
  ringbuffer_readptr = 0;

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

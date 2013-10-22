#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "uart_io/uart_io.h"
#include "serial_message/serial_message.h"

void wd_en();
void wd_dis();

void blink(uint8_t times);

void swu_receive_callback(uint8_t data) {
  SW_UART_datarecv_cb_unregister();

  serial_message_t *msg = sm_serial_message_new();
  msg->msg_type = data;
  msg->len = SW_UART_Receive();

  uint8_t *payload = malloc(msg->len);
  swu_receive(payload, msg->len);
  sm_append_payload(msg, payload, msg->len);
  free(payload);

  if(msg->msg_type < 0xb0) {
    sm_transmit_msg(UART, msg);
  }

  sm_serial_message_free(msg);
  SW_UART_datarecv_cb_register(swu_receive_callback);
}

void uart_receive_callback(uint8_t data) {
  uart_datarecv_cb_unregister();

  serial_message_t *msg = sm_serial_message_new();
  msg->msg_type = data;
  uart_receive(&(msg->len), 1);

  uint8_t *payload = malloc(msg->len);
  uart_receive(payload, msg->len);
  sm_append_payload(msg, payload, msg->len);
  free(payload);

  if(msg->msg_type < 0xb0) {
    sm_transmit_msg(SWU, msg);
  }

  sm_serial_message_free(msg);
  uart_datarecv_cb_register(uart_receive_callback);
}

int main(void) {

  sei();
  uart_init();
  SW_UART_Enable();

  SW_UART_datarecv_cb_register(swu_receive_callback);
  uart_datarecv_cb_register(uart_receive_callback);

  wd_en();

  while(1) {}
}

void wd_en() {
  WDTCR |= _BV(WDP0);
  WDTCR |= _BV(WDE);
}

void wd_dis(){
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;
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

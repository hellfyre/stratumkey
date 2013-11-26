#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "uart_io/uart_io.h"
#include "serial_message/serial_message.h"

void wd_en();
void wd_dis();

void blink(uint8_t times);

void swu_receive_callback() {

  serial_message_t msg;
  sm_receive_msg(SWU, &msg);

  if(msg.type < 0xb0) {
    sm_transmit_msg(UART, &msg);
  }

}

void uart_receive_callback() {

  serial_message_t msg;
  sm_receive_msg(UART, &msg);

  if(msg.type < 0xb0) {
    sm_transmit_msg(SWU, &msg);
  }

}

int main(void) {

  // Fiepen ausschalten
  DDRB = _BV(PB1);
  DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
  PORTB &= ~_BV(PB1);
  PORTD &= ~_BV(PD4) & ~_BV(PD5);

  sei();
  uart_init();
  SW_UART_Enable();

  SW_UART_datarecv_cb_register(swu_receive_callback);
  uart_datarecv_cb_register(uart_receive_callback);

  //wd_en();

  while(1) {}
}

/*
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
*/

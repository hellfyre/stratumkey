#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "uart_io/uart_io.h"

void wd_en();
void wd_dis();

void blink(uint8_t times);

void swu_receive_callback(uint8_t *data, uint8_t len) {
  if(data[0]<0xb0) {
    uart_transmit(data, len);
  }
}

void uart_receive_callback(uint8_t *data, uint8_t len) {
  if(data[0]<0xb0) {
    swu_transmit(data, len);
  }
}

int main(void) {

  sei();
  uart_init();
  SW_UART_Enable();

  swu_datarecv_cb_register(swu_receive_callback);
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
  WDTRC = 0x00;
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

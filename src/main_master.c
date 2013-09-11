#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "uart_io/uart_io.h"

void wd_en();
void wd_dis();

void blink(uint8_t times);

void receive_callback(uint8_t *data) {
}

int main(void) {
  uart_init();

  sei();
  SW_UART_Enable();

  swu_datarecv_cb_register(receive_callback);

  /*----- Outer loop -----*/
  while(1) {

    uint8_t challenge[32];
    uint8_t response[32];
    uint8_t id[2];
    uint8_t buffer = 0x00;

    /*----- Wait for start condition -----*/
    while(buffer != 0x99) {
      while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}
      buffer = SW_UART_Receive();
    }
    buffer = 0x01;
    uart_transmit(&buffer, 1);

    wd_en();

    /*----- Receive ID -----*/
    swu_receive(id, 2); // from key
    uart_transmit(id, 2); // to host

    wd_dis();

    /*----- Transmit challenge -----*/
    uart_receive(challenge, 32); // from host
    swu_transmit(challenge, 32); // to key

    wd_en();

    /*----- Receive response -----*/
    swu_receive(response, 32);
    uart_transmit(response, 32);

    wd_dis();
  }
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

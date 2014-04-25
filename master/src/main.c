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

#define LED_INIT DDRD |= _BV(PD6) | _BV(PD7);
#define LED_G_EN PORTD |= _BV(PD7);
#define LED_G_DIS PORTD &= ~_BV(PD7);
#define LED_R_EN PORTD |= _BV(PD6);
#define LED_R_DIS PORTD &= ~_BV(PD6);

// define 'State' func ptr
typedef void (*State)(void);

typedef enum events {
  SWU_RECVD,
  UART_RECVD,
  THREE
} Event;

State state;
serial_message_t msg;

// define state functions
void state_idle(Event ev) {
  if (ev == SWU_RECVD) {
    sm_receive_msg(SWU, &msg);
    sm_transmit_msg(UART, &msg);
  }
  else if (ev == UART_RECVD) {
    sm_receive_msg(UART, &msg);
    sm_transmit_msg(SWU, &msg);
  }
  sm_clear_msg(&msg);
}

int main(void) {

  sei();
  uart_init();
  SW_UART_Enable();

  while(1) {
    if ( READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL) ) {
      state(SWU_RECVD);
    }
    if ( uart_msg_waiting() ) {
      state(UART_RECVD);
    }
  }

}

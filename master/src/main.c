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

#define BUFSIZE 48

uint8_t swu_buffer[BUFSIZE+1];
uint8_t uart_buffer[BUFSIZE+1];
serial_message_t msg;

void reset_buffer(uint8_t *buffer) {
  for (int i=0; i<BUFSIZE; i++) buffer[i] = 0;
  buffer[BUFSIZE] = 1;
}

int main(void) {

  sei();
  reset_buffer(swu_buffer);
  uart_init();
  SW_UART_Enable();

  while(1) {
    while ( READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL) ) {
      swu_buffer[swu_buffer[BUFSIZE]] = SW_UART_Receive();
      swu_buffer[BUFSIZE]++;
      if ( swu_buffer[BUFSIZE] > 1 &&
           swu_buffer[BUFSIZE] == (swu_buffer[1]-2) ) {
        sm_deserialize(swu_buffer, &msg);
        reset_buffer(swu_buffer);
        sm_transmit_msg(UART, &msg);
        sm_clear_msg(&msg);
        break; // just in case
      }
    }
    while ( uart_msg_waiting() ) {
      uart_receive(&uart_buffer[uart_buffer[BUFSIZE]], 1);
      uart_buffer[BUFSIZE]++;
      if ( uart_buffer[BUFSIZE] > 1 &&
           uart_buffer[BUFSIZE] == (uart_buffer[1]-2) ) {
        sm_deserialize(uart_buffer, &msg);
        reset_buffer(uart_buffer);
        sm_transmit_msg(SWU, &msg);
        sm_clear_msg(&msg);
        break; // just in case
      }
    }
  }

}

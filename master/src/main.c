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

#ifndef FALSE
  #define FALSE 0
  #define TRUE !FALSE
#endif
#define RECVD_SWU recvd & 0x01
#define RECVD_UART recvd & 0x02
#define RECVD_SWU_SET recvd |= 0x01
#define RECVD_UART_SET recvd |= 0x02
#define RECVD_SWU_UNSET recvd &= ~0x01
#define RECVD_UART_UNSET recvd &= ~0x02

#define LED_INIT DDRD |= _BV(PD6) | _BV(PD7);
#define LED_G_EN PORTD |= _BV(PD7);
#define LED_G_DIS PORTD &= ~_BV(PD7);
#define LED_R_EN PORTD |= _BV(PD6);
#define LED_R_DIS PORTD &= ~_BV(PD6);

uint8_t recvd = 0;
serial_message_t msg;

void swu_receive_callback() {

  LED_G_EN
  sm_receive_msg(SWU, &msg);
  RECVD_SWU_SET;

}

void uart_receive_callback() {

  LED_R_EN
  sm_receive_msg(UART, &msg);
  RECVD_UART_SET;

}

int main(void) {

  sei();
  uart_init();
  SW_UART_Enable();

  SW_UART_datarecv_cb_register(swu_receive_callback);
  uart_datarecv_cb_register(uart_receive_callback);

  LED_INIT
  LED_R_EN
  _delay_ms(1000);
  LED_R_DIS
  LED_G_EN
  _delay_ms(1000);
  LED_G_DIS

  while(1) {
    if (RECVD_SWU) {
      sm_transmit_msg(UART, &msg);
      sm_clear_msg(&msg);
      RECVD_SWU_UNSET;
      _delay_ms(1000);
      LED_G_DIS
    }
    if (RECVD_UART) {
      sm_transmit_msg(SWU, &msg);
      sm_clear_msg(&msg);
      RECVD_UART_UNSET;
      _delay_ms(1000);
      LED_R_DIS
    }
  }

}

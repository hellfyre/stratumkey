#include <util/delay.h>

#include "single_wire_uart/swu_highlevel.h"
#include "single_wire_uart/single_wire_UART.h"

void swu_receive(uint8_t *data, uint8_t length) {
  for (int i=0; i<length; i++) {
    while(!READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL)) {}
    data[i] = SW_UART_Receive();
  }
}

void swu_transmit(uint8_t *data, uint8_t length) {
  for (int i=0; i<length; i++) {
    SW_UART_Transmit(data[i]);
    _delay_ms(TX_DELAY);
  }
}

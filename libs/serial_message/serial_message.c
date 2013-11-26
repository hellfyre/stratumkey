#include <string.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#ifdef STRATUMKEY_UART
  #include "uart_io/uart_io.h"
#endif

#include "serial_message.h"

void sm_append_payload(serial_message_t *msg, uint8_t *data, uint8_t len) {
  memcpy(msg->payload+msg->len, data, len);
  msg->len += len;
}

void sm_clear_msg(serial_message_t *msg) {
  msg->type = 0;
  msg->len = 0;
}

void sm_serialize(serial_message_t *msg, uint8_t *data) {
  memcpy(data, &(msg->type), 1);
  memcpy(data+1, &(msg->len), 1);
  memcpy(data+2, msg->payload, msg->len);
}

void sm_deserialize(uint8_t *data, serial_message_t *msg) {
  msg->type = data[0];
  msg->len = data[1];
  memcpy(msg->payload, data+2, msg->len);
}

void sm_transmit_msg(sm_line_t line, serial_message_t *msg) {
  uint8_t data[SM_OVERHEAD + SM_MAX_PAYLOAD];
  sm_serialize(msg, data);
  if (line == SWU) {
    swu_transmit(data, msg->len + SM_OVERHEAD);
  }
#ifdef STRATUMKEY_UART
  else if (line == UART) {
    uart_transmit(data, msg->len + SM_OVERHEAD);
  }
#endif
}

void sm_receive_msg(sm_line_t line, serial_message_t *msg) {
  if (line == SWU) {
    SW_UART_ringbuffer_read(&(msg->type), 1);
    SW_UART_ringbuffer_read(&(msg->len), 1);
    SW_UART_ringbuffer_read(msg->payload, msg->len);
  }
#ifdef STRATUMKEY_UART
  else if (line == UART) {
    uart_ringbuffer_read(&(msg->type), 1);
    uart_ringbuffer_read(&(msg->len), 1);
    uart_ringbuffer_read(msg->payload, msg->len);
  }
#endif
}

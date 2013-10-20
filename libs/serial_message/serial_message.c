#include <stdlib.h>
#include <string.h>

#include "serial_message.h"

serial_message_t* sm_serial_message_new() {
  serial_message_t *msg = malloc(sizeof(serial_message_t));
  msg->msg_type = 0;
  msg->len = 0;
  msg->payload = NULL;
  return msg;
}

void sm_serial_message_free(serial_message_t *msg) {
  if (msg->payload) sm_delete_payload(msg);
  free(msg);
}

uint8_t sm_message_length(serial_message_t *msg) {
  return msg->len + SM_OVERHEAD;
}

uint8_t sm_payload_length(serial_message_t *msg) {
  return msg->len;
}

void sm_append_payload(serial_message_t *msg, uint8_t *data, uint8_t len) {
  msg->payload = realloc(msg->payload, len+msg->len);
  memcpy(msg->payload+msg->len, data, len);
  msg->len += len;
}

void sm_delete_payload(serial_message_t *msg) {
  free(msg->payload);
  msg->len = 0;
}

uint8_t* sm_serialize(serial_message_t *msg) {
  uint8_t *data = malloc(msg->len + SM_OVERHEAD);
  memcpy(data, &(msg->msg_type), 1);
  memcpy(data+1, &(msg->len), 1);
  memcpy(data+2, msg->payload, msg->len);
  return data;
}

void sm_deserialize(serial_message_t *msg, uint8_t *data) {
  msg->msg_type = data[0];
  msg->len = data[1];
  uint8_t *payload = malloc(msg->len);
  memcpy(payload, data+2, msg->len);
  msg->payload = payload;
}

void sm_transmit_msg(sm_line_t line, serial_message *msg) {
  if (line == UART) {
    uart_transmit(sm_serialize(msg), sm_message_length(msg));
  }
  else if (line == SWU) {
    swu_transmit(sm_serialize(msg), sm_message_length(msg));
  }
}

serial_message_t* sm_receive_msg(sm_line_t line) {
  serial_message_t *msg = malloc(sizeof(serial_message_t));
  swu_receive(&(msg->msg_type), 1);
  swu_receive(&(msg->len), 1);
  swu_receive(msg->payload, msg->len);
  return msg;
}

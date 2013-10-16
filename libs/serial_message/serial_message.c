#include <stdlib.h>
#include <string.h>

#include "serial_message.h"

uint8_t sm_message_length(serial_message *msg) {
  return msg->len + SM_OVERHEAD;
}

uint8_t sm_payload_length(serial_message *msg) {
  return msg->len;
}

void sm_serialize(serial_message *msg, uint8_t *data) {
  data = malloc(msg->len + SM_OVERHEAD);
  memcpy(data, *msg->msg_type, 1);
  memcpy(data+1, *msg->len, 1);
  memcpy(data+2, *msg->payload, msg->len);
  return data;
}

void sm_deserialize(serial_message *msg, uint8_t *data) {
  msg->msg_type = data[0];
  msg->len = data[1];
  uint8_t *payload = malloc(msg->len);
  memcpy(payload, data+2, msg->len);
  msg->payload = payload;
}

#ifndef __SERIAL_MESSAGE__
#define __SERIAL_MESSAGE__

#include <stdint.h>

#define SM_OVERHEAD 2; # msg_type + len

typedef struct {
  uint8_t msg_type;
  uint8_t len;
  uint8_t *payload;
} serial_message_t;

uint8_t sm_message_length(serial_message_t *msg);
uint8_t sm_payload_length(serial_message_t *msg);
void sm_serialize(serial_message_t *msg, uint8_t *data);
void sm_deserialize(serial_message_t *msg, uint8_t *data);

#endif /* __SERIAL_MESSAGE__ */

#ifndef __SERIAL_MESSAGE__
#define __SERIAL_MESSAGE__

#include <stdint.h>

#define SM_OVERHEAD 2 //msg_type + len
#define SM_MAX_PAYLOAD 48

typedef enum {SWU, UART} sm_line_t;
typedef struct {
  uint8_t type;
  uint8_t len;
  uint8_t payload[SM_MAX_PAYLOAD];
} serial_message_t;

void sm_append_payload(serial_message_t *msg, uint8_t *data, uint8_t len);
void sm_clear_msg(serial_message_t *msg);
void sm_serialize(serial_message_t *msg, uint8_t *data);
void sm_deserialize(uint8_t *data, serial_message_t *msg);
void sm_transmit_msg(sm_line_t line, serial_message_t *msg);
void sm_receive_msg(sm_line_t line, serial_message_t *msg);

#endif /* __SERIAL_MESSAGE__ */

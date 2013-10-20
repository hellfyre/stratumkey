#ifndef __SERIAL_MESSAGE__
#define __SERIAL_MESSAGE__

#include <stdint.h>

#define SM_OVERHEAD 2 //msg_type + len

typedef enum {SWU, UART} sm_line_t;
typedef struct {
  uint8_t msg_type;
  uint8_t len;
  uint8_t *payload;
} serial_message_t;

serial_message_t* sm_serial_message_new();
void sm_serial_message_free(serial_message_t *msg);
uint8_t sm_message_length(serial_message_t *msg);
uint8_t sm_payload_length(serial_message_t *msg);
void sm_append_payload(serial_message_t *msg, uint8_t *data, uint8_t len);
void sm_delete_payload(serial_message_t *msg);
uint8_t* sm_serialize(serial_message_t *msg);
void sm_deserialize(serial_message_t *msg, uint8_t *data);
void sm_transmit_msg(sm_line_t line, serial_message_t *msg);
serial_message_t* sm_receive_msg(sm_line_t line);

#endif /* __SERIAL_MESSAGE__ */

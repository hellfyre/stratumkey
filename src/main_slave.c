#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "avrcryptolib/sha256.h"
#include "eeprom_io/eeprom_io.h"
#include "serial_message/serial_message.h"

#ifndef FALSE
  #define FALSE 0
  #define TRUE !FALSE
#endif

uint8_t key1[32];
uint8_t key2[32];
uint8_t uuid[16];
uint8_t success = FALSE;
sha256_hash_t hash;

int main(void) {
  eeprom_load(key1,  0, 32);
  eeprom_load(key2, 32, 32);
  eeprom_load(uuid, 64, 16);

  sei();
  SW_UART_Enable();

  while(!success) {
    /*----- Transmit ID + key2 -----*/
    serial_message_t *msg = sm_serial_message_new();

    msg->msg_type = 0xa1;
    sm_append_payload(msg, uuid, sizeof(uuid));
    sm_append_payload(msg, key2, sizeof(key2));

    sm_transmit_msg(SWU, msg);
    sm_serial_message_free(msg);

    /*----- Receive challenge -----*/
    msg = sm_receive_msg(SWU);

    if (msg->msg_type != 0xa2) continue;

    /*----- Concatenate challenge and key1 and hash the result -----*/
    uint8_t chall_key1[64];

    memcpy(chall_key1, msg->payload, 32);
    memcpy(chall_key1+32, key1, 32);
    sha256(&hash, chall_key1, 512);

    /*----- Transmit response -----*/
    sm_delete_payload(msg);
    
    msg->msg_type = 0xa3;
    sm_append_payload(msg, hash, sizeof(hash));
    sm_transmit_msg(SWU, msg);
    success = TRUE;
  }
}

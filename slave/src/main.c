#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"
#include "eeprom_io/eeprom_io.h"
#include "serial_message/serial_message.h"

#define LED_INIT DDRB |= _BV(PB4);
#define LED_EN PORTB |= _BV(PB4);
#define LED_DIS PORTB &= ~_BV(PB4);

// define 'State' func ptr
typedef void (*State)(void);

typedef enum events {
  NOTHING,
  MSG_RECVD,
  TIMEOUT
} Event;

State state;
serial_message_t msg;

uint8_t counter = 0;

uint8_t key1[32];
uint8_t key2[32];
uint8_t uuid[16];
sha256_hash_t hash;

// define state functions
void state_init(Event ev) {
  if(ev == NOTHING) {
    //----- Transmit ID + key2 -----//
    sm_clear_msg(&msg);
    msg.type = 0xa1;
    sm_append_payload(&msg, uuid, sizeof(uuid));
    sm_append_payload(&msg, key2, sizeof(key2));

    sm_transmit_msg(SWU, &msg);
    sm_clear_msg(&msg);

    state = state_id_sent;
    counter = 0;
  }
}

void state_id_sent(Event ev) {
  if(ev == MSG_RECVD) {

    sm_receive_msg(SWU, &msg);
    if (msg.type != 0xa2) {
      state = state_init;
      sm_clear_msg(&msg);
      return;
    }

    //----- Concatenate challenge and key1 and hash the result -----//
    uint8_t chall_key1[64];

    memcpy(chall_key1, msg.payload, 32);
    memcpy(chall_key1+32, key1, 32);
    sha256(&hash, chall_key1, 512);

    //----- Transmit response -----//
    sm_clear_msg(&msg);
    msg.type = 0xa3;
    sm_append_payload(&msg, hash, sizeof(hash));
    sm_transmit_msg(SWU, &msg);

    //----- Phase 3: Profit! -----//
    state = state_success;
  }
  else if(ev == TIMEOUT) {
    state = state_init;
  }

}

void state_success(Event ev) {}

int main(void) {
  eeprom_load(key1,  0, 32);
  eeprom_load(key2, 32, 32);
  eeprom_load(uuid, 64, 16);

  sei();
  SW_UART_Enable();

  while(state != state_success) {
    if( READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL) ) {
      state(MSG_RECVD);
    }
    else {
      
      _delay_ms(100);

      counter++;
      if(counter == 20) {
        counter = 0;
        state(TIMEOUT);
      }
      else {
        state(NOTHING);
      }

    }
  }

}

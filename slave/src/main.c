#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "single_wire_uart/single_wire_UART.h"
#include "single_wire_uart/swu_highlevel.h"
#include "avrcryptolib/sha256.h"
#include "eeprom_io/eeprom_io.h"
#include "serial_message/serial_message.h"

#ifndef FALSE
  #define FALSE 0
  #define TRUE !FALSE
#endif

#define RECVD_SWU recvd & 0x01
#define RECVD_SWU_SET recvd |= 0x01
#define RECVD_SWU_UNSET recvd &= ~0x01

#define LED_INIT DDRB |= _BV(PB4);
#define LED_EN PORTB |= _BV(PB4);
#define LED_DIS PORTB &= ~_BV(PB4);

uint8_t key1[32];
uint8_t key2[32];
uint8_t uuid[16];
uint8_t recvd = 0;
uint8_t success = FALSE;
sha256_hash_t hash;

void swu_receive_callback() {
}

int main(void) {
  eeprom_load(key1,  0, 32);
  eeprom_load(key2, 32, 32);
  eeprom_load(uuid, 64, 16);

  sei();
  SW_UART_Enable();
  //SW_UART_datarecv_cb_register(swu_receive_callback);

  LED_INIT
  _delay_ms(200);

  //while(!success) {
    //----- Transmit ID + key2 -----//
    serial_message_t msg;

    msg.type = 0xa1;
    sm_append_payload(&msg, uuid, sizeof(uuid));
    sm_append_payload(&msg, key2, sizeof(key2));

    sm_transmit_msg(SWU, &msg);
    sm_clear_msg(&msg);

    //----- Receive challenge -----//
    /*
    while(1) {
      //PINB |= _BV(PB4);
      if (RECVD_SWU) {
        _delay_ms(500);
        LED_DIS
        RECVD_SWU_UNSET;
        break;
      }
    }
    */
    _delay_ms(3000);
    sm_receive_msg(SWU, &msg);
    for (int i=0; i<msg.len; i++) {
      msg.payload[i]++;
    }
    sm_transmit_msg(SWU, &msg);

    /*
    uint8_t debug_answer[4];
    debug_answer[0] = 0xde;
    debug_answer[1] = 0xad;
    debug_answer[2] = 0xbe;
    debug_answer[3] = 0xef;
    msg.type = 0xa3;
    sm_append_payload(&msg, debug_answer, sizeof(debug_answer));
    sm_transmit_msg(SWU, &msg);
    */
    sm_clear_msg(&msg);
    //swu_transmit(debug_answer, 4);

    // DEBUG
    //_delay_ms(2000);
    //continue;
    // DEBUG

    //if (msg.type != 0xa2) continue;

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
    success = TRUE;
  //}
}

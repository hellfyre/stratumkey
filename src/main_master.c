#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include <main_master.h>
#include <1wire/1wire.h>
#include <avrcryptolib/sha256.h>

uint8_t secret[32];
uint8_t challenge[32];
uint8_t response[32];

void challenge_response_cycle();


int main(void) {
  owi_init(_BV(WIREPIN));

  while (1) {
    uint8_t presence = 0;
    presence = owi_detectpresence(_BV(WIREPIN));
    if (presence > 0) {
      _delay_us(100);
      owi_sendbyte(0xaf, _BV(WIREPIN));
    }
    _delay_ms(500);
  }

  int i;
  for (i=0; i<4; i++) {
    secret[i] = 0xaf;
  }
}

void challenge_response_cycle() {
  uint8_t presence = 0;
  presence = owi_detectpresence(_BV(WIREPIN));
  if (presence > 0) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(1000);
    PORTB = 0;
    _delay_ms(1000);
  }

  // TODO check return value of owi_detectpresence

  // generate random challenge
  int i,j;
  for (i=0; i<8; i++) {
    unsigned long random_single = random();
    for (j=(i*4); j<(i*4+4); j++) {
      challenge[j] = random_single & 0xff;
      random_single >>= 8;
    }
  }

  // send challenge
  for (i=0; i<32; i++) {
    owi_sendbyte(challenge[i], _BV(WIREPIN));
  }

  // wait for the slave to process the challenge
  // TODO count instructions on slave
  _delay_us(20);

  // receive response
  for (i=0; i<32; i++) {
    response[i] = owi_receivebyte(_BV(WIREPIN));
  }

  // TODO check response
  // like this: if $response = $(sha256($secret & $challenge [& $random_slave])
  // $random_slave isn't actually needed, it's just plain paranoia
}

#include <avr/io.h>
#include <avr/interrupt.h>

#include <1wire/1wire.h>
#include <avrcryptolib/sha256.h>

#define WIREPIN PB0

#define STATE_IDLE 0x00
#define STATE_CHALLENGE 0x01
#define STATE_RESPONSE 0x02

uint8_t secret[32];
uint8_t challenge[32];

SIGNAL(SIG_INTERRUPT0) {
  owi_init(1<<WIREPIN);
  challenge_response_cycle();
}

int main(void) {
  int i;
  for (i=0; i<4; i++) {
    secret[i] = 0xaf;
  }
}

void challenge_response_cycle() {
  owi_detectpresence(1<<WIREPIN);
  // TODO check return value of owi_detectpresence
  int i,j;
  for (i=0; i<8; i++) {
    unsigned long random_single = random();
    for (j=(i*4); j<(i*4+4); j++) {
      challenge[j] = random_single & 0xff;
      random_single >>= 8;
    }
  }

  for (i=0; i<32; i++) {
    owi_sendbyte(challenge[i], (1<<WIREPIN));
  }
}

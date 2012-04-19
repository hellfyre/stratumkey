#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include <main_slave.h>
#include <avrcryptolib/sha256.h>

#define BUFSIZE 32
#define CR_LENGTH 0xff

void decode_bitwise(uint8_t interval);
void next_state();

uint8_t state = STATE_RESET;
uint8_t rxtx_buf[BUFSIZE];
//uint8_t random_val[BUFSIZE];
uint8_t secret[BUFSIZE];
uint8_t rxtx_buf_pos = 0;
uint8_t bits_remaining;

SIGNAL(SIG_INTERRUPT0) {
  uint8_t timeval;
  uint8_t wire = OWI_PIN;

  if ( (wire & _BV(WIREPIN)) == _BV(WIREPIN) ) {
    timeval = TCNT;
    TCR = 0;
    TCNT = 0;
    decode_bitwise(timeval);
  }
  else {
    TCR = TDIV;
  }
}

int main(void) {
  MCUCR = _BV(ISC00);

  #ifdef attiny13
  GIMSK = _BV(PCIE);
  PCMSK = _BV(PCINT0);
  #endif
  #ifdef atmega8
  GICR = _BV(INT0);
  #endif
  sei();

  int i;
  for (i=0; i<4; i++) {
    secret[i] = 0xaf;
  }

  while(1) {}
}

void decode_bitwise(uint8_t interval) {
  // sanity check: is our interval valid?
  // TODO do real life test to determine whether we need this
  if (interval == 0) return;
  if (interval > 15 && interval < 60) return;
  if (interval > 120 && interval < 230) return;

  // reset
  if (interval >= 230) {
    cli();
    OWI_PULL_BUS_LOW(_BV(WIREPIN));
    _delay_us(OWI_DELAY_I_STD_MODE);
    OWI_RELEASE_BUS(_BV(WIREPIN));

    state = STATE_CHALLENGE;
    //bits_remaining = CR_LENGTH;
    bits_remaining = 8;
  }
  else { // write0, write1 or read
    uint8_t rx_msb = rxtx_buf[rxtx_buf_pos] & 0x01; // save the LSB of the current buffer byte, just in case we're in read mode
    rxtx_buf[rxtx_buf_pos] >>= 1;

    if (interval < 20) { // write1 or read
      if (IS_READMODE(state)){
        if (rx_msb) {
          OWI_PULL_BUS_LOW(_BV(WIREPIN));
          _delay_us(OWI_DELAY_J_STD_MODE);
          OWI_RELEASE_BUS(_BV(WIREPIN));
        }
      }
      else { // if we are in write mode, write a 1 to the MSB
        rxtx_buf[rxtx_buf_pos] |= 0x80;
      }
    }
    if (bits_remaining == 0) {
      next_state();
    }
    if ( (bits_remaining % 8) == 0) {
      rxtx_buf_pos = (rxtx_buf_pos + 1) % BUFSIZE;
    }
    bits_remaining--;
  }
}

void next_state() {
  if (rxtx_buf[0] == 0xaf) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(500);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(500);
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(500);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(500);
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(500);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(500);
  }

  return;

  if (state == STATE_CHALLENGE) {
    // This would be the "client also sends random value" paranoia function
    /*
    int i,j;
    for (i=0; i<8; i++) {
      unsigned long random_single = random();
      for (j=(i*4); j<(i*4+4); j++) {
        random_val[j] = random_single & 0xff;
        rxtx_buf[j] &= random_val[j] & secret[j];
        random_single >>= 8;
      }
    }
    */

    //sha256(&rxtx_buf, rxtx_buf, 256);
    state = STATE_RESPONSE;
  }
}

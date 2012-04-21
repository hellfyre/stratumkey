#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include <main_slave.h>
#include <avrcryptolib/sha256.h>

#define BITS 7

uint8_t state = STATE_RESET;
uint8_t rxtx_buf[32];
uint8_t secret[32];
uint8_t rxtx_buf_pos = 0;
uint8_t bits_remaining;

SIGNAL(SIG_INTERRUPT0) {
  if (IS_READMODE(state)) {
    uint8_t temp = rxtx_buf[rxtx_buf_pos] & 0x01;

    if (!temp) {
      OWS_PULL_BUS(WIREPIN);
      _delay_us(15);
      OWS_RELEASE_BUS(WIREPIN);
    }
    rxtx_buf[rxtx_buf_pos] >>= 1;

    if ( bits_remaining == 0 ) {
      next_state();
    }
    else if ( (bits_remaining % 8) == 0 ) {
      rxtx_buf_pos++;
    }
    bits_remaining--;
  }
  else if (state > 0) {
    _delay_us(19);

    rxtx_buf[rxtx_buf_pos] >>= 1;
    uint8_t wire = OWS_PIN;

    if ( (wire & _BV(WIREPIN)) != _BV(WIREPIN) ) {
      rxtx_buf[rxtx_buf_pos] |= 0x80;
    }

    if ( bits_remaining == 0 ) {
      next_state();
    }
    else if ( (bits_remaining % 8) == 0 ) {
      rxtx_buf_pos++;
    }
    bits_remaining--;
  }
  else {
    ow_init();
  }

}

int main(void) {
  MCUCR = _BV(ISC01) | _BV(ISC00);

  #ifdef attiny13
  GIMSK = _BV(PCIE);
  PCMSK = _BV(PCINT0);
  #endif
  #ifdef atmega8
  GICR = _BV(INT0);
  #endif
  sei();

  while(1) {}
}

void ow_init() {
  uint8_t wire;
  
  cli();

  _delay_us(160);
  wire = OWS_PIN;

  // if the wire is still high, then the master currently resets the bus, which
  // is what we expect in STATE_RESET
  if ( (wire & _BV(WIREPIN)) == _BV(WIREPIN) ) {
    do { // wait for the master to release the bus
      _delay_us(5);
      wire = OWS_PIN;
    } while ( (wire & _BV(WIREPIN)) == _BV(WIREPIN) );
    
    OWS_PULL_BUS(_BV(WIREPIN));
    _delay_us(80);
    OWS_RELEASE_BUS(_BV(WIREPIN));
    sei();
  }

  state = STATE_CHALLENGE;
  bits_remaining = BITS;
}

void next_state() {
  if (state == STATE_CHALLENGE) {
    state = STATE_RESPONSE;
    bits_remaining = BITS;
  }
}

void peak() {
  DDRB = _BV(PB6);
  PORTB = _BV(PB6);
  _delay_us(1);
  DDRB = 0;
  PORTB = 0;
}

void morse_byte(uint8_t data) {
  int i;
  for (i=0; i<8; i++) {
    uint8_t temp = data & 0x01;
    if (temp) {
      DDRB = _BV(PB6);
      PORTB = _BV(PB6);
      _delay_us(50);
      DDRB = 0;
      PORTB = 0;
      _delay_us(10);
    }
    else {
      DDRB = _BV(PB6);
      PORTB = _BV(PB6);
      _delay_us(10);
      DDRB = 0;
      PORTB = 0;
      _delay_us(50);
    }
    data >>= 1;
  }
}

void blink(uint8_t times) {
  int i;
  for (i=0; i<times; i++) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(500);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(500);
  }
  _delay_ms(500);
}

void blink_long(uint8_t times) {
  int i;
  for (i=0; i<times; i++) {
    DDRB = _BV(PB6);
    PORTB = _BV(PB6);
    _delay_ms(2000);
    DDRB = 0;
    PORTB = 0;
    _delay_ms(2000);
  }
  _delay_ms(2000);
}

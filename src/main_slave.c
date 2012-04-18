#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include <util/delay.h>

#include <avrcryptolib/sha256.h>

// One-wire bus definitions

#define OWI_PULL_BUS_LOW(bitMask) \
            OWI_DDR |= bitMask; \
            OWI_PORT &= ~bitMask;

#define OWI_RELEASE_BUS(bitMask) \
            OWI_DDR &= ~bitMask; \
            OWI_PORT |= bitMask;

// Bit timing delays in us
#define     OWI_DELAY_A_STD_MODE    6
#define     OWI_DELAY_B_STD_MODE    64
#define     OWI_DELAY_C_STD_MODE    60
#define     OWI_DELAY_D_STD_MODE    10
#define     OWI_DELAY_E_STD_MODE    9
#define     OWI_DELAY_F_STD_MODE    55
//#define     OWI_DELAY_G_STD_MODE  0
#define     OWI_DELAY_H_STD_MODE    480
#define     OWI_DELAY_I_STD_MODE    70
#define     OWI_DELAY_J_STD_MODE    40

// MCU specific defines
#define     OWI_PORT        PORTB
#define     OWI_PIN         PINB
#define     OWI_DDR         DDRB

#define WIREPIN PB0

#define BUFSIZE 32
#define CR_LENGTH 0xff

// 8-bit-state, MSB indicates read state
#define STATE_RESET 0x00
#define STATE_CHALLENGE 0x81
#define STATE_RESPONSE 0x02
#define IS_READMODE(state) (state>>7)

void decode_bitwise(uint8_t interval);
void next_state();

uint8_t state = STATE_RESET;
uint8_t rxtx_buf[BUFSIZE];
//uint8_t random_val[BUFSIZE];
uint8_t secret[BUFSIZE];
uint8_t rxtx_buf_pos = 0;
uint8_t bits_remaining;

SIGNAL(SIG_INTERRUPT0) {
  uint8_t wire_high = (PINB>>WIREPIN) & 0x01;
  uint8_t timeval;
  if (wire_high) {
    timeval = TCNT0;
    TCCR0B = 0;
    TCNT0 = 0;
    decode_bitwise(timeval);
  }
  else {
    TCCR0B = (1<<CS01);
  }
}

int main(void) {
  // First of all: pull line up (or down) to trigger IRQ at master
  DDRB = (1<<WIREPIN);
  PORTB = (1<<WIREPIN);
  DDRB &= ~(1<<WIREPIN);

  GIMSK = (1<<PCIE);
  PCMSK = (1<<PCINT0);
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
  if (interval > 120 && interval < 480) return;

  // reset
  if (interval >= 480) {
    OWI_PULL_BUS_LOW(1<<WIREPIN);
    _delay_us(OWI_DELAY_I_STD_MODE);
    OWI_RELEASE_BUS(1<<WIREPIN);

    state = STATE_CHALLENGE;
    bits_remaining = CR_LENGTH;
  }
  else { // write0, write1 or read
    uint8_t rx_msb = rxtx_buf[rxtx_buf_pos] & 0x01; // save the LSB of the current buffer byte, just in case we're in read mode
    rxtx_buf[rxtx_buf_pos] >>= 1;

    if (interval < 20) { // write1 or read
      if (IS_READMODE(state)){
        if (rx_msb) {
          OWI_PULL_BUS_LOW(1<<WIREPIN);
          _delay_us(OWI_DELAY_J_STD_MODE);
          OWI_RELEASE_BUS(1<<WIREPIN);
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

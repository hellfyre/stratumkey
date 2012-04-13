#include <avr/io.h>
#include <avr/interrupt.h>

#include <sha256.h>
#include <1wire.h>

#define F_CPU 1200000UL
#include <util/delay.h>

// 8-bit-state, MSB indicates read state
#define STATE_RESET 0x00
#define STATE_CHALLENGE 0x81
#define STATE_RESPONSE 0x02
#define IS_READMODE(state) (state>>7)

#define CR_LENGTH 32
#define BUFSIZE 4
#define WIREPIN PB0

void decode_bitwise(uint8_t interval);

uint8_t state = STATE_RESET;
uint8_t rxtx_buf[BUFSIZE];
uint8_t rxtx_buf_pos = 0;
uint8_t bits_remaining;

SIGNAL(SIG_PIN_CHANGE0) {
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

  while(1) {}
}

void decode_bitwise(uint8_t interval) {
  // sanity check: is our interval valid?
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
    uint8_t rx_msb = rxtx_buf[rxtx_buf_pos] >> 7; // save the MSB of the current buffer byte, just in case we're in read mode
    rxtx_buf[rxtx_buf_pos] <<= 1;

    if (interval < 20) { // write1 or read
      if (IS_READMODE(state)){
        if (rx_msb) {
          OWI_PULL_BUS_LOW(1<<WIREPIN);
          _delay_us(OWI_DELAY_J_STD_MODE);
          OWI_RELEASE_BUS(1<<WIREPIN);
        }
      }
      else {
        rxtx_buf[rxtx_buf_pos] |= 0x01;
      }
    }
    bits_remaining--;
    if (bits_remaining == 0) {
      //next_state();
    }
    if ( (bits_remaining % 8) == 0) {
      rxtx_buf_pos = (rxtx_buf_pos + 1) % BUFSIZE;
    }
  }
}

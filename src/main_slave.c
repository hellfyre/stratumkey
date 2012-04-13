#include <avr/io.h>
#include <avr/interrupt.h>

#include <sha256.h>

#define F_CPU 1200000UL
#include <util/delay.h>

// 8-bit-state, MSB indicates read state
#define STATE_RESET 0x00
#define STATE_CHALLENGE 0x81
#define STATE_RESPONSE 0x02

#define CR_LENGTH 32
#define BUFSIZE 4
#define WIREPIN PB0
#define WIREDIR DDB0

uint8_t state = STATE_RESET;
uint8_t rx_buf[BUFSIZE];
uint8_t rx_buf_pos = 0;
uint8_t tx_buf[BUFSIZE];
uint8_t tx_buf_pos = 0;
uint8_t bits_remaining;

SIGNAL(SIG_PIN_CHANGE0) {
  uint8_t wire_high = (PINB>>WIREPIN) & 0x01;
  uint8_t timeval;
  if (wire_high) {
    timeval = TCNT0;
    TCCR0B = 0;
    TCNT0 = 0;
  }
  else {
    TCCR0B = (1<<CS01);
  }
}

int main(void) {
  // First of all: pull line up (or down) to trigger IRQ at master
  DDRB = (1<<WIREDIR);
  PORTB = (1<<WIREPIN);
  DDRB &= ~(1<<WIREDIR);

  GIMSK = (1<<PCIE);
  PCMSK = (1<<PCINT0);
  sei();

  while(1) {}
}

void idle_state() {
  DDRB &= ~(1<<WIREDIR);
  PORTB = (1<<WIREPIN);
}

void decode_bitwise(uint8_t timerval) {
  if (timerval >= 480) { // reset
    switch_to_output();
    _delay_us(OWI_DELAY_I_STD_MODE);
    switch_to_input();
    state = STATE_CHALLENGE;
    bits_remaining = CR_LENGTH;
  }
  else if (!is_readmode()) {
    if ( (timerval >= 1 && timerval <= 15) || (timerval >= 60 && timerval <= 120) ) {
      tx_buf[tx_buf_pos] << 1;
      if (timerval <= 15) {
        tx_buf[tx_buf_pos] |= 0x01;
      }
      tx_buf_bit++;
      if (tx_buf_bit == 8) {
        uint8_t tx_buf_pos_tmp = tx_buf_pos;
        tx_buf_pos = (tx_buf_pos + 1) % BUFSIZE;
        tx_buf_bit = 0;
        process_byte(tx_buf_pos_tmp);
      }
    }
  }
  /*
  else if (timerval >= 1 && timerval <= 15) { // write1 or read
    if (READMODE) {
    }
    else {
      tx_buf[tx_buf_pos] << 1;
    }
  }
  else if (timerval >= 60 && timerval <= 120) { // write0
    tx_buf[tx_buf_pos] << 1;
  }
  */
}

void switch_to_output() {
  DDRB = (1<<WIREDIR);
  PORTB = 0;
}

void switch_to_input() {
  DDRB &= ~(1<<WIREDIR);
  PORTB = (1<<WIREPIN);
}

uint8_t is_readmode() {
  return (state>>7);
}

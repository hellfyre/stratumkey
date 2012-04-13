#include <avr/io.h>
#include <avr/interrupt.h>

#include <sha256.h>

#define F_CPU 1200000UL
#include <util/delay.h>

#define BUFSIZE 32
#define WIREPIN PB0
#define WIREDIR DDB0

uint8_t READMODE = 0;
uint8_t rx_buf[BUFSIZE];
uint8_t tx_buf[BUFSIZE];
uint8_t tx_buf_pos = 0;

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
  }
  else if (timerval >= 1 && timerval <= 15) { // write1 or read
    if (READMODE) {
    }
    else {
    }
  }
  else if (timerval >= 60 && timerval <= 120) { // write0
  }
}

void switch_to_output() {
  DDRB = (1<<WIREDIR);
  PORTB = 0;
}

void switch_to_input() {
  DDRB &= ~(1<<WIREDIR);
  PORTB = (1<<WIREPIN);
}

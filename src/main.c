#include <avr/io.h>

#include <sha256.h>

#define F_CPU 8000000UL
#include <util/delay.h>

int main(void) {
  while(1) {
    _delay_ms(5);
  }
}

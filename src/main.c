#include <avr/io.h>

#include <sha256.h>
#include <i2csoft.h>

#define F_CPU 8000000UL
#include <util/delay.h>

int main(void) {
  SoftI2CInit();
  while(1) {
    SoftI2CStart();
    SoftI2CWriteByte(0xde);
    SoftI2CStop();
    
    _delay_ms(2);
  }
}

#ifndef __MAIN_MASTER_H__
#define __MAIN_MASTER_H__

#ifdef attiny13
#define WIREPIN PB0
#endif

#ifdef atmega8
#define WIREPIN PD2
#endif

#define STATE_IDLE 0x00
#define STATE_CHALLENGE 0x01
#define STATE_RESPONSE 0x02

void peak();
void morse_byte(uint8_t data);
void blink(uint8_t times);
void blink_long(uint8_t times);

#endif /*  __MAIN_MASTER_H__ */

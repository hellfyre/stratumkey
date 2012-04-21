#ifndef __MAIN_SLAVE_H__
#define __MAIN_SLAVE_H__

// One-wire bus definitions
// MCU specific defines
#ifdef attiny13
#define     OWS_PORT        PORTB
#define     OWS_PIN         PINB
#define     OWS_DDR         DDRB
#define     WIREPIN         PB0
#define     OCR             OCR0A
#define     TCR             TCCR0B
#define     TCNT            TCNT0
#define     TDIV            _BV(CS01)
#endif

#ifdef atmega8
#define     OWS_PORT        PORTD
#define     OWS_PIN         PIND
#define     OWS_DDR         DDRD
#define     WIREPIN         PD2
#define     OCR             OCR2
#define     TCR             TCCR2
#define     TCNT            TCNT2
#define     TDIV            _BV(CS20) | _BV(WGM21)
#endif

#define OWS_PULL_BUS(bitMask) \
            OWS_DDR |= bitMask; \
            OWS_PORT |= bitMask;

#define OWS_RELEASE_BUS(bitMask) \
            OWS_DDR &= ~bitMask; \
            OWS_PORT &= ~bitMask;

// 8-bit-state, MSB indicates read state
#define STATE_RESET 0x00
#define STATE_CHALLENGE 0x01
#define STATE_RESPONSE 0x82
#define IS_READMODE(state) (state>>7)

void ow_init();
void next_state();
void peak();
void morse_byte(uint8_t data);
void blink(uint8_t times);
void blink_long(uint8_t times);

#endif /* __MAIN_SLAVE_H__ */

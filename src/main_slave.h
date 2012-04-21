#ifndef __MAIN_SLAVE_H__
#define __MAIN_SLAVE_H__

// One-wire bus definitions
// Bit timing delays in us
#define     OWS_DELAY_A_STD_MODE    6
#define     OWS_DELAY_B_STD_MODE    64
#define     OWS_DELAY_C_STD_MODE    60
#define     OWS_DELAY_D_STD_MODE    10
#define     OWS_DELAY_E_STD_MODE    9
#define     OWS_DELAY_F_STD_MODE    55
//#define     OWS_DELAY_G_STD_MODE  0
#define     OWS_DELAY_H_STD_MODE    240
#define     OWS_DELAY_I_STD_MODE    70
#define     OWS_DELAY_J_STD_MODE    40

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

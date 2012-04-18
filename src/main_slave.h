#ifndef __MAIN_SLAVE_H__
#define __MAIN_SLAVE_H__

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
#ifdef attiny13
#define     OWI_PORT        PORTB
#define     OWI_PIN         PINB
#define     OWI_DDR         DDRB
#define     WIREPIN         PB0
#define     TCR             TCCR0B
#define     TCNT            TCNT0
#define     TDIV            _BV(CS01)
#endif

#ifdef atmega8
#define     OWI_PORT        PORTD
#define     OWI_PIN         PIND
#define     OWI_DDR         DDRD
#define     WIREPIN         PD2
#define     TCR             TCCR2
#define     TCNT            TCNT2
#define     TDIV            _BV(CS20)
#endif

// 8-bit-state, MSB indicates read state
#define STATE_RESET 0x00
#define STATE_CHALLENGE 0x81
#define STATE_RESPONSE 0x02
#define IS_READMODE(state) (state>>7)

#endif /* __MAIN_SLAVE_H__ */

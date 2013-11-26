#ifndef __STRATUMKEY_DEVDEF_H__
#define __STRATUMKEY_DEVDEF_H__

// Baud rate settings (WAIT_ONE - PRESCALER):
//  Baud Rate     1MHz      2Mhz      4MHz      8MHz
//     4800     207 - 1    51 - 8   103 - 8   (207 - 8)
//     9600     103 - 1   207 - 1    51 - 8   103 - 8
//    19200       NA      103 - 1   207 - 1    51 - 8
//    28800       NA         NA     138 - 1    34 - 8
//    38400       NA         NA     103 - 1   207 - 1
// Please note that the UART consumes about all CPU resources when WAIT_ONE*PRESCALER<100.

/* ---------- attiny85 8MHz ------------------------------------------------- */

/* Communication parameters. The WAIT_ONE definiton has to be changed according to equation 2-1 in the application note. */
#define WAIT_ONE             103      //!< Half bit period compare setting. See the application note for calculation of this value. Make sure timer prescaler is set to the intended value.
#define PRESCALER             8       //!< Prescaler setting. Must be set according to the baud rate setting.

/* Port and pin settings. */
#define SW_UART_PIN_NUMBER    PB2     //!< Set pin number for communication.
#define SW_UART_PORT          PORTB   //!< Set port for communication.
#define SW_UART_PIN           PINB    //!< Set pin for communication.
#define SW_UART_DDR           DDRB    //!< Data direction register. Not available for high voltage ports.

/* Timer prescaler device defines */
#define TIMER_PRESCALER_CONTROL_REGISTER    TCCR0B //!< Define the timer control register according to the timer used for the UART.
#define TIMER_PRESCALER_1                   CS00
#define TIMER_PRESCALER_8                   CS01

/* UART interrupt vectors definitions. */
#define SW_UART_EXTERNAL_INTERRUPT_VECTOR       INT0_vect             //!< UART external interrupt vector. Make sure this is in accordance to the defined UART pin.
#define SW_UART_TIMER_COMPARE_INTERRUPT_VECTOR  TIMER0_COMPA_vect      //!< UART compare interrupt vector.

/* Timer device defines */
#define TIMER_INT_MASK    TIMSK
#define TIMER_INT_FLAG    TIFR
#define TIMER_CONTROL     TCCR0A
#define WAVEFORM_BIT      WGM01
#define TIMER_COUNT       TCNT0
#define OUTPUT_COMPARE    OCR0A
#define OUTPUT_COMP_INT   OCIE0A
#define OUTPUT_COMP_FLAG  OCF0A

/* Interrupt device defines */
#define MCU_CONTROL       MCUCR
#define INT_FLAG          GIFR
#define INT_SENSE         ISC01       //< Sets falling edge of INT0 generates interrupt.
#define EXT_INT_BIT       INT0
#define EXT_INT_FLAG      INTF0
#define EN_EXT_INT        GIMSK

#endif /*__STRATUMKEY_DEVDEF_H__*/

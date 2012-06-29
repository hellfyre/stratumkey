#ifndef __SWU_DEVICE_DEFINES_H__
#define __SWU_DEVICE_DEFINES_H__

/* ---------- atmega8 8MHz -------------------------------------------------- */
#ifdef atmega8

/* Communication parameters. The WAIT_ONE definiton has to be changed according to equation 2-1 in the application note. */
#define WAIT_ONE             103      //!< Half bit period compare setting. See the application note for calculation of this value. Make sure timer prescaler is set to the intended value.
#define PRESCALER             8       //!< Prescaler setting. Must be set according to the baud rate setting.

/* Port and pin settings. */
#define SW_UART_PIN_NUMBER    PD2     //!< Set pin number for communication.
#define SW_UART_PORT          PORTD   //!< Set port for communication.
#define SW_UART_PIN           PIND    //!< Set pin for communication.
#define SW_UART_DDR           DDRD    //!< Data direction register. Not available for high voltage ports.

/* Timer prescaler device defines */
#define TIMER_PRESCALER_CONTROL_REGISTER    TCCR2 //!< Define the timer control register according to the timer used for the UART.
#define TIMER_PRESCALER_1                   CS20
#define TIMER_PRESCALER_8                   CS21

/* UART interrupt vectors definitions. */
#define SW_UART_EXTERNAL_INTERRUPT_VECTOR       INT0_vect             //!< UART external interrupt vector. Make sure this is in accordance to the defined UART pin.
#define SW_UART_TIMER_COMPARE_INTERRUPT_VECTOR  TIMER2_COMP_vect      //!< UART compare interrupt vector.

/* Timer device defines */
#define TIMER_INT_MASK    TIMSK
#define TIMER_INT_FLAG    TIFR
#define TIMER_CONTROL     TCCR2
#define WAVEFORM_BIT      WGM21
#define TIMER_COUNT       TCNT2
#define OUTPUT_COMPARE    OCR2
#define OUTPUT_COMP_INT   OCIE2
#define OUTPUT_COMP_FLAG  OCF2

/* Interrupt device defines */
#define MCU_CONTROL       MCUCR
#define INT_FLAG          GIFR
#define INT_SENSE         ISC01       //< Sets falling edge of INT0 generates interrupt.
#define EXT_INT_BIT       INT0
#define EXT_INT_FLAG      INTF0
#define EN_EXT_INT        GICR

#endif

/* ---------- attiny85 8MHz ------------------------------------------------- */
#ifdef attiny85

/* Communication parameters. The WAIT_ONE definiton has to be changed according to equation 2-1 in the application note. */
#define WAIT_ONE             103      //!< Half bit period compare setting. See the application note for calculation of this value. Make sure timer prescaler is set to the intended value.
#define PRESCALER             8       //!< Prescaler setting. Must be set according to the baud rate setting.

/* Port and pin settings. */
#define SW_UART_PIN_NUMBER    PB3     //!< Set pin number for communication.
#define SW_UART_PORT          PORTB   //!< Set port for communication.
#define SW_UART_PIN           PINB    //!< Set pin for communication.
#define SW_UART_DDR           DDRB    //!< Data direction register. Not available for high voltage ports.

/* Timer prescaler device defines */
#define TIMER_PRESCALER_CONTROL_REGISTER    TCCR0B //!< Define the timer control register according to the timer used for the UART.
#define TIMER_PRESCALER_1                   CS00
#define TIMER_PRESCALER_8                   CS01

/* UART interrupt vectors definitions. */
#define SW_UART_EXTERNAL_INTERRUPT_VECTOR       INT0_vect             //!< UART external interrupt vector. Make sure this is in accordance to the defined UART pin.
#define SW_UART_TIMER_COMPARE_INTERRUPT_VECTOR  TIMER0_COMP_vect      //!< UART compare interrupt vector.

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
#endif

#endif /*__SWU_DEVICE_DEFINES_H__*/

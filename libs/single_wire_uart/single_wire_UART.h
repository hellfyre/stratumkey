/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      This is the header file for the single-wire software UART.
 *
 *      The file contains definitions and macros for the single-wire software
 *      UART. If a different microcontroller than the ATmega32 is used, this
 *      file needs to be changed. Port and baud rate settings may also be
 *      changed in this file.
 *
 * \par Application note:
 *      AVR274: Single-wire Software UART
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Name:  $
 * $Revision: 1364 $
 * $RCSfile: single_wire_UART.h,v $
 * $Date: 2007-02-08 14:13:02 +0100 (to, 08 feb 2007) $  \n
 ******************************************************************************/

#include "stdint.h"     //Integer types.
#include "swu_device_defines.h"

#ifndef FALSE
  #define FALSE 0
  #define TRUE !FALSE
#endif

// Baud rate settings (WAIT_ONE - PRESCALER):
//  Baud Rate     1MHz      2Mhz      4MHz      8MHz
//     4800     207 - 1    51 - 8   103 - 8   (207 - 8)
//     9600     103 - 1   207 - 1    51 - 8   103 - 8
//    19200       NA      103 - 1   207 - 1    51 - 8
//    28800       NA         NA     138 - 1    34 - 8
//    38400       NA         NA     103 - 1   207 - 1
// Please note that the UART consumes about all CPU resources when WAIT_ONE*PRESCALER<100.

#define CB_RECV_BUFFER_SIZE 64

#define TRANSMIT_DELAY        70    //!< Cycles from the start bit is sent (from UART_transmit) to the timer is started plus cycles in the timer interrupt before first data bit is sent.
#define RECEIVE_DELAY         76    //!< Cycles from the start bit is detected to the timer is started plus cycles in timer interrupt before first data bit is received.

#define WAIT_ONEHALF          (WAIT_ONE + WAIT_ONE/2)

#if (PRESCALER == 1)
  #define START_UART_TIMER()     (TIMER_PRESCALER_CONTROL_REGISTER |= (1<<TIMER_PRESCALER_1))
  #define STOP_UART_TIMER()      (TIMER_PRESCALER_CONTROL_REGISTER &= ~(1<<TIMER_PRESCALER_1))
#elif (PRESCALER == 8)
  #define START_UART_TIMER()     (TIMER_PRESCALER_CONTROL_REGISTER |= (1<<TIMER_PRESCALER_8))
  #define STOP_UART_TIMER()      (TIMER_PRESCALER_CONTROL_REGISTER &= ~(1<<TIMER_PRESCALER_8))
#else
  #error PRESCALER must be set to 1 or 8
#endif

#if ( ((WAIT_ONEHALF-(RECEIVE_DELAY/PRESCALER)) > 255) || ((WAIT_ONE) > 255))
  #error WAIT_ONE is set too high. Try to increase prescaler or use a higher baud rate.
#endif

#if ( (WAIT_ONE) < (100/PRESCALER))
  #error A too high baud rate is used. Please check the PRESCALER and WAIT_ONE setting.
#endif

/* Pin macros.  */
#define INITIALIZE_UART_PIN()   ( SW_UART_PORT &= ~(1<<SW_UART_PIN_NUMBER) )    //!< Clear port.
#define READ_UART_PIN()         ( SW_UART_PIN & (1<<SW_UART_PIN_NUMBER) )

/* Macros for standard AVR ports. */
#define SET_UART_PIN()          ( SW_UART_DDR &= ~(1<<SW_UART_PIN_NUMBER) )     //!< Tri-state pin.
#define CLEAR_UART_PIN()        ( SW_UART_DDR |= (1<<SW_UART_PIN_NUMBER) )      //!< Set pin output low.
/* Macros for high voltage AVR ports. */
//#define SET_UART_PIN()          ( SW_UART_PORT &= ~(1<<SW_UART_PIN_NUMBER) )//!< Tri-state pin.
//#define CLEAR_UART_PIN()        ( SW_UART_PORT |= (1<<SW_UART_PIN_NUMBER) ) //!< Set pin output low.

/* Timer macros. These are device dependent. */
#define CLEAR_UART_TIMER_ON_COMPARE_MATCH()     (TIMER_CONTROL |= (1<<WAVEFORM_BIT))                        //!< Set timer control register to clear timer on compare match (CTC).
#define SET_UART_TIMER_COMPARE_WAIT_ONE()       (OUTPUT_COMPARE = WAIT_ONE)                                 //!< Sets the timer compare register to one period.
#define SET_UART_TIMER_COMPARE_START_TRANSMIT() (OUTPUT_COMPARE = WAIT_ONE - (TRANSMIT_DELAY/PRESCALER))    //!< Sets the timer compare register to the correct value when a transmission is started.
#define SET_UART_TIMER_COMPARE_START_RECEIVE()  (OUTPUT_COMPARE = WAIT_ONEHALF - (RECEIVE_DELAY/PRESCALER)) //!< Sets the timer compare register to the correct value when a reception is started.
#define CLEAR_UART_TIMER()                      (TIMER_COUNT = 0x00)
#define ENABLE_UART_TIMER_INTERRUPT()           (TIMER_INT_MASK |= (1<<OUTPUT_COMP_INT))
#define DISABLE_UART_TIMER_INTERRUPT()          (TIMER_INT_MASK &= ~(1<<OUTPUT_COMP_INT))
#define CLEAR_UART_TIMER_INTERRUPT_FLAG()       (TIMER_INT_FLAG = (1<<OUTPUT_COMP_FLAG))

/* External interrupt macros. These are device dependent. */
#define INITIALIZE_UART_EXTERNAL_INTERRUPT()    (MCU_CONTROL |= (1<<INT_SENSE)); \
						(INT_FLAG = (1<<EXT_INT_FLAG))  //< Sets falling edge of EXT_INT_BIT generates interrupt
#define ENABLE_UART_EXTERNAL_INTERRUPT()       	(INT_FLAG = (1<<EXT_INT_FLAG)); \
						(EN_EXT_INT |= (1<<EXT_INT_BIT)) // clear interrupt flag befor enabling it
#define DISABLE_UART_EXTERNAL_INTERRUPT()       (EN_EXT_INT &= ~(1<<EXT_INT_BIT) )
#define CLEAR_UART_EXTERNAL_INTERRUPT_FLAG()    (INT_FLAG = (1<<EXT_INT_FLAG))

/* Status register defines. */
#define SW_UART_TX_BUFFER_FULL        4     //!< Set if data is ready to be sent from the Tx buffer.
#define SW_UART_RX_BUFFER_FULL        5     //!< Set if data is ready to be received from the Rx buffer.
#define SW_UART_RX_BUFFER_OVERFLOW    6     //!< Set if receive buffer is overflowed. Indicates data loss.
#define SW_UART_FRAME_ERROR           7     //!< Set if a frame error has occured.

/* Flag macros. */
#define SET_FLAG(flag_register,flag_bit)    ( (flag_register) |= (1 << (flag_bit)) )  //!< Use this macro when setting a flag in a register.
#define CLEAR_FLAG(flag_register,flag_bit)  ( (flag_register) &= ~(1 << (flag_bit)) ) //!< Use this macro when clearing a flag in a register.
#define READ_FLAG(flag_register,flag_bit)   ( (flag_register) & (1 << (flag_bit)) )   //!< Use this macro when reading a flag in a register.


extern volatile uint8_t SW_UART_status;         //!< Byte holding status flags.

//Use this to put the UART_counter in a dedicated register. Remember to comment out the previous declarations (also in the
//declaration in single_wire_UART.c) of the counter variable and to lock register 15 when this is used. This is done under
//the code section in the C/C++ compiler category in the IAR options menu.
//__no_init __regvar volatile uint8_t SW_UART_status @ 15;

//Use this to put the UART_status in a dedicated register. Remember to comment out the previous declaration of the
//counter variable and to lock register 14 when this is used. This is done under the code section in the C/C++ compiler
//category in the IAR options menu.
//__no_init __regvar volatile uint8_t UART_counter @ 14;

//Use this to put the UART_status in a general purpose io register. Check device datasheet to find adress of GPIO register if available
//(set to 0x1E here). Comment out the previos declarations (also in the declaration in single_wire_UART.c) of the status variable
//when the GPIO register is used.
//__io __no_init static volatile uint8_t SW_UART_status @ 0x1E;

// Single wire UART data received callback
typedef void (*swu_datarecv_cb_t)(uint8_t *data);
extern swu_datarecv_cb_t swu_datarecv_callback; // one callback is plenty
extern uint8_t swu_receive_buffer[CB_RECV_BUFFER_SIZE];
extern uint8_t swu_receive_buffer_ctr;

void swu_datarecv_accumulate(uint8_t data);
void swu_datarecv_cb_register(swu_datarecv_cb_t cb);
void swu_datarecv_cb_dispatch(uint8_t *data);

/* Global UART functions. */
void    SW_UART_Enable(void);       //!< Enable the UART.
void    SW_UART_Disable(void);      //!< Disable the UART.
void    SW_UART_Transmit(uint8_t);  //!< Transmit one byte.
uint8_t SW_UART_Receive(void);      //!< Receive one byte.

#ifndef __UART_IO_H__
#define __UART_IO_H__

#include <stdint.h>

#include "uart_io/uart_device_defines.h"

void uart_init();
uint8_t uart_msg_waiting();
void uart_receive(uint8_t *data, uint8_t length);
void uart_transmit(uint8_t *data, uint8_t length);

#endif /*__UART_IO_H__*/

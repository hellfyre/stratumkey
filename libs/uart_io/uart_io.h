#ifndef __UART_IO_H__
#define __UART_IO_H__

void uart_init();
void uart_receive(uint8_t *data, uint8_t length);
void uart_transmit(uint8_t *data, uint8_t length);

#endif /*__UART_IO_H__*/

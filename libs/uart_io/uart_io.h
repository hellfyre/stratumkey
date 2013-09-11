#ifndef __UART_IO_H__
#define __UART_IO_H__

// USART data received callback
typedef void (*uart_datarecv_cb_t)(uint8_t *data);
extern uart_datarecv_cb_t uart_datarecv_callback; // one callback is plenty
extern uint8_t uart_receive_buffer[CB_RECV_BUFFER_SIZE];
extern uint8_t uart_receive_buffer_ctr;

void uart_datarecv_accumulate(uint8_t data);
void uart_datarecv_cb_register(uart_datarecv_cb_t cb);
void uart_datarecv_cb_dispatch(uint8_t *data);

void uart_init();
void uart_receive(uint8_t *data, uint8_t length);
void uart_transmit(uint8_t *data, uint8_t length);

#endif /*__UART_IO_H__*/

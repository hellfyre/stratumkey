#ifndef __SWU_HIGHLEVEL_H__
#define __SWU_HIGHLEVEL_H__

#include "single_wire_UART.h"

#define TX_DELAY 2

void swu_receive(uint8_t *data, uint8_t length);
uint16_t swu_receive_id();
void swu_transmit(uint8_t *data, uint8_t length);

#endif /*__SWU_HIGHLEVEL_H__*/

#ifndef __MAIN_SLAVE_H__
#define __MAIN_SLAVE_H__

// 8-bit-state, MSB indicates read state
#define STATE_RESET 0x00
#define STATE_CHALLENGE 0x01
#define STATE_RESPONSE 0x82
#define IS_READMODE(state) (state>>7)

void peak();
void morse_byte(uint8_t data);
void blink(uint8_t times);
void blink_long(uint8_t times);

#endif /* __MAIN_SLAVE_H__ */

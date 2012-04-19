#ifndef __1WIRE_H__
#define __1WIRE_H__

#include <avr/io.h>

#define OWI_CMD_ROM_READ	0x33
#define OWI_CMD_ROM_SKIP	0xCC
#define OWI_CMD_ROM_MATCH	0x55
#define OWI_CMD_ROM_SEARCH	0xF0

#define EOK			0
#define EFAILED			1

// Custom defines
#define     OWI_PORT        PORTD
#define     OWI_PIN         PIND
#define     OWI_DDR         DDRD

// Bit timing delays in us
#define     OWI_DELAY_A_STD_MODE    6
#define     OWI_DELAY_B_STD_MODE    64
#define     OWI_DELAY_C_STD_MODE    60
#define     OWI_DELAY_D_STD_MODE    10
#define     OWI_DELAY_E_STD_MODE    9
#define     OWI_DELAY_F_STD_MODE    55
//#define     OWI_DELAY_G_STD_MODE  0
#define     OWI_DELAY_H_STD_MODE    240
#define     OWI_DELAY_I_STD_MODE    70
#define     OWI_DELAY_J_STD_MODE    40

uint8_t owi_lock(uint8_t pins);
void owi_unlock(uint8_t pins);

void owi_init(uint8_t pins);
void owi_writebit0(uint8_t pins);
void owi_writebit1(uint8_t pins);
uint8_t owi_readbit(uint8_t pins);
uint8_t owi_detectpresence(uint8_t pins);

void owi_sendbyte(uint8_t data, uint8_t pins);
uint8_t owi_receivebyte(uint8_t pins);

void owi_skiprom(uint8_t pins);
void owi_readrom(uint8_t *rom, uint8_t pin);
void owi_matchrom(uint8_t *rom, uint8_t pins);
uint8_t owi_searchrom(uint8_t *pattern, uint8_t lastdev, uint8_t pins);
uint8_t owi_getdevice(uint8_t *id, uint8_t family, uint8_t bus);

#define OWI_PULL_BUS_LOW(bitMask) \
            OWI_DDR |= bitMask; \
            OWI_PORT &= ~bitMask;

#define OWI_RELEASE_BUS(bitMask) \
            OWI_DDR |= bitMask; \
            OWI_PORT |= bitMask;

#endif /* __1WIRE_H__ */

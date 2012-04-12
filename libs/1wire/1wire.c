/* 1wire software implementation
 *
 * mostly taken from Atmel's application note
 *
 * (c) 2012 by Daniel Willmann <daniel@totalueberwachung.de>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "1wire.h"

#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

static uint8_t lock;

uint8_t owi_lock(uint8_t pins)
{
  if ((lock&pins) == 0) {
    lock |= pins;
    return 0;
  } else return lock&pins;
}

void owi_unlock(uint8_t pins)
{
  lock &= ~pins;
}

void owi_init(uint8_t pins)
{
  lock &= ~(pins);
  OWI_RELEASE_BUS(pins);
  _delay_us(OWI_DELAY_H_STD_MODE);
}

void owi_writebit0(uint8_t pins)
{
  uint8_t intstate;

  intstate = SREG;
  cli();

  OWI_PULL_BUS_LOW(pins);
  _delay_us(OWI_DELAY_C_STD_MODE);

  OWI_RELEASE_BUS(pins);
  _delay_us(OWI_DELAY_D_STD_MODE);

  SREG = intstate;
}

void owi_writebit1(uint8_t pins)
{
  uint8_t intstate;

  intstate = SREG;
  cli();

  OWI_PULL_BUS_LOW(pins);
  _delay_us(OWI_DELAY_A_STD_MODE);

  OWI_RELEASE_BUS(pins);
  _delay_us(OWI_DELAY_B_STD_MODE);

  SREG = intstate;
}

uint8_t owi_readbit(uint8_t pins)
{
  uint8_t intstate, bitread;

  intstate = SREG;
  cli();

  OWI_PULL_BUS_LOW(pins);
  _delay_us(OWI_DELAY_A_STD_MODE);

  OWI_RELEASE_BUS(pins);
  _delay_us(OWI_DELAY_E_STD_MODE);

  bitread = OWI_PIN & pins;
  _delay_us(OWI_DELAY_F_STD_MODE);

  SREG = intstate;

  return bitread;
}

uint8_t owi_detectpresence(uint8_t pins)
{
  uint8_t intstate;
  uint8_t presence;

  intstate = SREG;
  cli();

  OWI_PULL_BUS_LOW(pins);
  _delay_us(OWI_DELAY_H_STD_MODE);

  OWI_RELEASE_BUS(pins);
  _delay_us(OWI_DELAY_I_STD_MODE);

  presence = ((~OWI_PIN) & pins);
  _delay_us(OWI_DELAY_F_STD_MODE);

  SREG = intstate;

  return presence;
}

void owi_sendbyte(uint8_t data, uint8_t pins)
{
  uint8_t temp, i;

  for (i=0; i<8; i++) {
    temp = data&0x01;
    if (temp) {
      owi_writebit1(pins);
    } else {
      owi_writebit0(pins);
    }

    data >>= 1;
  }
}

uint8_t owi_receivebyte(uint8_t pin)
{
  uint8_t data, i;

  data = 0x00;

  for (i=0;i<8;i++) {
    data >>= 1;
    if (owi_readbit(pin)) {
      data |= 0x80;
    }
  }
  return data;
}

void owi_skiprom(uint8_t pins)
{
  owi_sendbyte(OWI_CMD_ROM_SKIP, pins);
}

void owi_readrom(uint8_t *rom, uint8_t pin)
{
  uint8_t bytesleft = 8;

  owi_sendbyte(OWI_CMD_ROM_READ, pin);

  while (bytesleft > 0) {
    *rom++ = owi_receivebyte(pin);
    bytesleft--;
  }
}

void owi_matchrom(uint8_t *rom, uint8_t pins)
{
  uint8_t bytesleft = 8;

  owi_sendbyte(OWI_CMD_ROM_MATCH, pins);

  while (bytesleft > 0) {
    owi_sendbyte(*rom++, pins);
    bytesleft--;
  }
}

uint8_t owi_searchrom(uint8_t *pattern, uint8_t lastdev, uint8_t pin)
{
  uint8_t currentbit = 1;
  uint8_t newdev = 0;
  uint8_t bitmask = 0x01;
  uint8_t bita, bitb;

  owi_sendbyte(OWI_CMD_ROM_SEARCH, pin);

  while (currentbit <= 64) {
    bita = owi_readbit(pin);
    bitb = owi_readbit(pin);

    if (bita && bitb) {
      newdev = -EFAILED;
      return newdev;
    } else if (bita ^ bitb) {
      if (bita) {
        (*pattern) |= bitmask;
      } else {
        (*pattern) &= ~bitmask;
      }
    } else {
      if (currentbit == lastdev) {
        (*pattern) |= bitmask;
      } else if (currentbit > lastdev) {
        (*pattern) &= ~bitmask;
        newdev = currentbit;
      } else if (!(*pattern & bitmask)) {
        newdev = currentbit;
      } else {
      }
    }

    if ((*pattern) &bitmask) {
      owi_writebit1(pin);
    } else {
      owi_writebit0(pin);
    }

    currentbit ++;

    bitmask <<= 1;
    if (!bitmask) {
      bitmask = 0x01;
      pattern++;
    }
  }
  return newdev;
}

uint8_t owi_getdevice(uint8_t *id, uint8_t family, uint8_t bus)
{
  uint8_t presence;
  uint8_t lastdev = 8;

  presence = owi_detectpresence(bus);

  memset(id, 0, 8);
  id[0] = family;

  lastdev = 7;
  if (bus & presence) {
    owi_detectpresence(bus);
    lastdev = owi_searchrom(id, lastdev, bus);
    if (lastdev == -EFAILED) {
      return -EFAILED;
    }
  } else {
    return -EFAILED;
  }
  if (id[0] != family) {
    return -EFAILED;
  }

// TODO: Implement CRC check
//  if (owi_checkromcrc(id) != EOK) {
//    return -EFAILED;
//  }
  return EOK;
}

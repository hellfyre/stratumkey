#ifndef __SWU_DEVICE_DEFINES_H__
#define __SWU_DEVICE_DEFINES_H__

#ifdef MCU_attiny85
  #include "stratumkey_attiny85.h"
#endif
#ifdef MCU_atmega8
  #include "stratumkey_atmega8.h"
#endif
#ifdef MCU_atmega328p
  #include "stratumkey_atmega328p.h"
#endif

#endif /*__SWU_DEVICE_DEFINES_H__*/

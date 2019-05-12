#ifndef CHECKSUM16_H // include guard
#define CHECKSUM16_H

#include <Arduino.h>

/*
Calculates XMODEM CRC16 against an array of bytes
*/

class CRC16 {
   public:
      static uint16_t CalculateArray(uint8_t data[], uint16_t length);
    private:
      static uint8_t reflect(uint8_t data, uint8_t bits = 32);
};
#endif

#ifndef Steinhart_H // include guard
#define Steinhart_H

#include <Arduino.h>

class Steinhart {
   public:
      static float ThermistorToCelcius(uint16_t BCOEFFICIENT, uint16_t RawADC);

/*
struct SteinhartValues {
  float A;
  float B;
  float C;
};
*/


};
#endif

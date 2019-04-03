/*
LICENSE
Attribution-NonCommercial-ShareAlike 2.0 UK: England & Wales (CC BY-NC-SA 2.0 UK)
https://creativecommons.org/licenses/by-nc-sa/2.0/uk/

* Non-Commercial — You may not use the material for commercial purposes.
* Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
  You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
* ShareAlike — If you remix, transform, or build upon the material, you must distribute your
  contributions under the same license as the original.
* No additional restrictions — You may not apply legal terms or technological measures
  that legally restrict others from doing anything the license permits.

  PIN MAPPINGS
  Diagram
  https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/extras/ATtiny_x41.md

  PA1 = PIN 12 SERIAL TRANSMIT (TXD0)
  PA2 = PIN 11 SERIAL RECEIVE (RXD0)

  PA3 = DUMP LOAD ENABLE / PIN 10 /  ARDUINO PIN 7/A3
  PA4 = ADC4 PIN 9 ARDUINO PIN 6/A4 = ON BOARD TEMP sensor
  PA5 = RED_LED / PIN 8 / ARDUINO PIN 5/A5  (SERIAL PORT 1 TXD1)
  PA6 = GREEN_LED / PIN 7 / ARDUINO PIN 4/A6
  PA7 = ADC7 = PIN 6 = ARDUINO PIN 3/A7 = 2.048V REFERENCE ENABLE

  PB2 = ADC8 PIN 5 ARDUINO PIN 2/A8 = VOLTAGE reading
  PB0 = ADC11 PIN 2 ARDUINO PIN 0/A11 = REMOTE TEMP sensor
  PB1 = ADC10 PIN 3 ARDUINO PIN 1/A10 = SPARE INPUT/OUTPUT

  //TODO: We should have mapped PA5 to spare PIN on sensor header its TXD1 and mapped RED_LED to PB1 to assist debugging

*/
#ifndef DIYBMS_DEFINES_H // include guard
#define DIYBMS_DEFINES_H

//This is where the data begins in EEPROM
#define EEPROM_CONFIG_ADDRESS 0

//#define VOLTAGE_PIN A8
//#define BOARD_TEMPERATURE_PIN A4

#define nop  __asm__("nop\n\t");

#ifdef DIYBMS_DEBUG

#define DEBUG_PRINT(str) \
   Serial1.print(__LINE__); \
   Serial1.print(' '); \
   Serial1.println(str);
#else
  #define DEBUG_PRINT(str);
#endif


//Default values
struct CellModuleConfig {
  uint8_t mybank;
  uint8_t BypassOverTempShutdown;
  uint16_t BypassThresholdmV;

  // Resistance of bypass load
  float LoadResistance;
  //Voltage Calibration
  float Calibration;
  //Reference voltage (millivolt) normally 2.00mV
  float mVPerADC;
  //Internal Thermistor settings
  uint16_t Internal_BCoefficient;
  //External Thermistor settings
  uint16_t External_BCoefficient;
} __attribute__((packed));

#endif

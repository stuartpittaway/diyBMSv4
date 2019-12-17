#ifndef Settings_H // include guard
#define Settings_H

/*

EEPROM Settings storage with checksum
(c)2019 Stuart Pittaway

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

ATTiny841 data sheet
http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8495-8-bit-AVR-Microcontrollers-ATtiny441-ATtiny841_Datasheet.pdf
*/

#include <EEPROM.h>

#include "crc16.h"


class Settings {
   public:
      static void WriteConfigToEEPROM(uint8_t* settings, uint16_t size, uint16_t eepromStartAddress);
      static bool ReadConfigFromEEPROM(uint8_t* settings, uint16_t size, uint16_t eepromStartAddress);
      static void FactoryDefault(uint16_t size,uint16_t eepromStartAddress);
};
#endif

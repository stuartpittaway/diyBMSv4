#include "settings.h"

void Settings::WriteConfigToEEPROM(uint8_t* settings, uint16_t size, uint16_t eepromStartAddress) {
  //TODO: We should probably check EEPROM.length() to ensure its big enough

  uint16_t EEPROMaddress=eepromStartAddress;
  for (uint16_t i = 0; i < size; i++) {
    EEPROM.update( EEPROMaddress, settings[i] );
    EEPROMaddress++;
  }

  //Generate and save the checksum for the setting data block
  //uint16_t checksum = uCRC16Lib::calculate(settings, size);
  uint16_t checksum = CRC16::CalculateArray(settings, size);
  EEPROM.put(eepromStartAddress+size, checksum);
}

bool Settings::ReadConfigFromEEPROM(uint8_t* settings, uint16_t size, uint16_t eepromStartAddress) {
  uint16_t EEPROMaddress=eepromStartAddress;
  for (uint16_t i = 0; i < size; i++) {
    settings[i]=EEPROM.read(EEPROMaddress);
    EEPROMaddress++;
  }

  // Calculate the checksum
  //uint16_t checksum = uCRC16Lib::calculate(settings, size);
  uint16_t checksum = CRC16::CalculateArray(settings, size);


  uint16_t existingChecksum;
  EEPROM.get(eepromStartAddress+size, existingChecksum);

  if (checksum == existingChecksum) {
    //Return TRUE
    return true;
  }

  //Original data is now corrupt so return FALSE
  return false;
}

void Settings::FactoryDefault(uint16_t size, uint16_t eepromStartAddress) {
  EEPROM.put(eepromStartAddress+size, 0x0000);
}

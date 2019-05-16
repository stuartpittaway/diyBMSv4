#include "settings.h"

void Settings::WriteConfigToEEPROM(char* settings, int size, uint16_t eepromStartAddress) {
  //Serial1.println("WriteConfigToEEPROM");

  //TODO: We should probably check EEPROM.length() to ensure its big enough
  EEPROM.begin(EEPROM_storageSize);

  uint16_t EEPROMaddress = eepromStartAddress;
  for (int i = 0; i < size; i++) {
    //Serial1.print(settings[i],HEX);
    EEPROM.put( EEPROMaddress, settings[i] );
    EEPROMaddress++;
  }

  //Serial1.print('=');
  //Generate and save the checksum for the setting data block
  uint16_t checksum = CRC16::CalculateArray((uint8_t*)settings, size);
  //Serial1.println(checksum,HEX);
  EEPROM.put(eepromStartAddress + size, checksum);
  EEPROM.end();
}

bool Settings::ReadConfigFromEEPROM(char* settings, int size, uint16_t eepromStartAddress) {
  //Serial1.println("ReadConfigFromEEPROM");
  EEPROM.begin(EEPROM_storageSize);

  uint16_t EEPROMaddress = eepromStartAddress;
  for (int i = 0; i < size; i++) {
    settings[i] = EEPROM.read(EEPROMaddress);
    //Serial1.print(settings[i],HEX);
    EEPROMaddress++;
  }

  // Calculate the checksum
  uint16_t checksum = CRC16::CalculateArray((uint8_t*)settings, size);
  uint16_t existingChecksum;
  EEPROM.get(eepromStartAddress + size, existingChecksum);

  //Serial1.print('=');  Serial1.print(existingChecksum,HEX);  Serial1.print('=');  Serial1.println(checksum,HEX);

  EEPROM.end();
  if (checksum == existingChecksum) {
    //Return TRUE
    return true;
  }

  //Original data is now corrupt so return FALSE
  return false;
}

void Settings::FactoryDefault(int size, uint16_t eepromStartAddress) {
  EEPROM.begin(EEPROM_storageSize);
  EEPROM.put(eepromStartAddress + size, 0x0000);
  EEPROM.end();
}

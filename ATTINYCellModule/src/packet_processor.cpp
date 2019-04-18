/*
____  ____  _  _  ____  __  __  ___
(  _ \(_  _)( \/ )(  _ \(  \/  )/ __)
)(_) )_)(_  \  /  ) _ < )    ( \__ \
(____/(____) (__) (____/(_/\/\_)(___/

DIYBMS V4.0
CELL MODULE FOR ATTINY841

(c)2019 Stuart Pittaway

COMPILE THIS CODE USING PLATFORM.IO

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
*/

#include "packet_processor.h"

void PacketProcessor::incrementPacketAddress() {
  buffer.address = (buffer.address & 0xF0) + ((buffer.address & 0x0F) + 1);
}

bool PacketProcessor::BypassOverheatCheck() {
  if (InternalTemperature() > _config->BypassOverTempShutdown ) {
      return true;
  }

  return false;
}

int8_t PacketProcessor::InternalTemperature() {
  return round(Steinhart::ThermistorToCelcius(_config->Internal_BCoefficient, onboard_temperature));
}

bool PacketProcessor::BypassCheck() {
  if (CellVoltage() > _config->BypassThresholdmV  ) {
      //We need to start bypass
      return true;
  }

  return false;
}

bool PacketProcessor::isPacketForMe() {
  //Modules can be grouped together in banks - only allow processing of packets in the correct bank
  if (((buffer.address & 0x30) >> 4) != _config->mybank) return false;

  //Broadcast for my bank?
  if ((buffer.address & 0x80) == 0x80) {
    //If we receive a broadcast message whatever address is received is my unique module address
    //so store it for later use
    mymoduleaddress = buffer.address & 0x0F;

    //Ensure the next module has a higher address
    incrementPacketAddress();
    return true;
  }

  //Is this packet addressed directly to me?
  if ((buffer.address & 0x0F) == mymoduleaddress && mymoduleaddress!=0xFF) return true;

  return false;
}

void PacketProcessor::ADCReading(uint16_t value) {
  switch (adcmode) {
  case ADC_CELL_VOLTAGE:
    {
      //UpdateRingBuffer(value);
      raw_adc_voltage=value;
      break;
    }
  case ADC_INTERNAL_TEMP:
    {
      onboard_temperature = value;
      break;
    }
  case ADC_EXTERNAL_TEMP:
    {
      external_temperature = value;
      break;
    }
  }
}

/*
void PacketProcessor::UpdateRingBuffer(uint16_t value) {
  //Pop off value from ring buffer
  ringtotal -= ringbuffer[ringptr];
  ringbuffer[ringptr] = value;
  ringtotal += ringbuffer[ringptr];
  ringptr++;
  if (ringptr >= ringsize) {
    ringptr = 0;
  }
}
*/

void PacketProcessor::TakeAnAnalogueReading(uint8_t mode) {
  adcmode = mode;

  switch (adcmode) {
  case ADC_CELL_VOLTAGE:
    {
      _hardware->SelectCellVoltageChannel();
      break;
    }
  case ADC_INTERNAL_TEMP:
    {
      _hardware->SelectInternalTemperatureChannel();
      break;
    }
  case ADC_EXTERNAL_TEMP:
    {
      _hardware->SelectExternalTemperatureChannel();
      break;
    }
  default:
    //Avoid taking a reading if we get to here
    return;
  }

  _hardware->BeginADCReading();
}

byte* PacketProcessor::GetBufferPointer() {
  return (byte*)&buffer;
}

int PacketProcessor::GetBufferSize() {
  return sizeof(buffer);
}

bool PacketProcessor::onPacketReceived(const uint8_t* receivebuffer, size_t len) {
  //if (len == 0) {    badpackets++;    return false;  }

  // Process your decoded incoming packet here.
  if (len == sizeof(buffer)) {

    //Copy to our buffer (probably a better way to share memory than this)
    memcpy(&buffer, receivebuffer, sizeof(buffer));

    //Calculate the CRC and compare to received
    uint16_t validateCRC = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2);

    if (validateCRC == buffer.crc) {
      //It's a good packet
      if (isPacketForMe()) {
          if (processPacket()) {

            //Set flag to indicate we processed packet
            buffer.command=buffer.command | B10000000;

            //Calculate new checksum over whole buffer
            buffer.crc = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2);
        }
      }
      return true;
    }
  }

  //We need to do something here, the packet received was not correct
  badpackets++;
  return false;
}

//uint16_t PacketProcessor::ReadRawRingValue() {
  //return ringtotal >> ringsize_bits;
//}

//Read cell voltage and return millivolt reading (16 bit unsigned)
uint16_t PacketProcessor::CellVoltage() {
  //TODO: Get rid of the need for float variables?
  float v = ((float) raw_adc_voltage * _config->mVPerADC) * _config->Calibration;

  return (uint16_t) v;
}

uint16_t PacketProcessor::RawADCValue() {
  return raw_adc_voltage;
}

uint8_t PacketProcessor::TemperatureToByte(float TempInCelcius) {
  //This function reduces the scale of temperatures from float types to a single byte (unsigned)
  //We have an artifical floor at 40oC, anything below +40 is considered negative (below freezing)
  //Gives range of -40 to +216 degrees C

  TempInCelcius += 40;

  //Set the limits
  if (TempInCelcius < 0) TempInCelcius = 0;
  if (TempInCelcius > 255) TempInCelcius = 255;

  return (uint8_t) TempInCelcius;
}

//command byte
// RRRR CCCC
// X    = 1 bit indicate if packet processed
// R    = 3 bits reserved not used
// C    = 4 bits command (16 possible commands)

//commands
// 0000 0000  = set bank
// 0000 0001  = read voltage and status
// 0000 0010  = identify module (flash leds)
bool PacketProcessor::processPacket() {
  switch (buffer.command & 0x0F) {

  case COMMAND::SetBankIdentity:
    {
      //Set this modules bank address and store in EEPROM
      _config->mybank = buffer.moduledata[mymoduleaddress] & 0x30;

      //TODO: Need to write into EEPROM....

      //Indicate we processed this packet
      buffer.moduledata[mymoduleaddress] = 0xFFFF;
      return true;
    }

  case COMMAND::ReadVoltageAndStatus:
    {
      //Read voltage of VCC
      //Maximum voltage 8191mV
      buffer.moduledata[mymoduleaddress] = CellVoltage() & 0x1FFF;

      //3 top bits
      //X = In bypass
      //Y = Bypass over temperature
      //Z = Not used

      if (BypassOverheatCheck()) {
        buffer.moduledata[mymoduleaddress]=buffer.moduledata[mymoduleaddress] | 0x4000;
      }

      if (WeAreInBypass) {
        buffer.moduledata[mymoduleaddress]=buffer.moduledata[mymoduleaddress] | 0x8000;
      }

      return true;
    }

  case COMMAND::Identify:
    {
      //identify module
      //Indicate that we received and did something
      buffer.moduledata[mymoduleaddress] = 0xFFFF;

      //For the next 10 receied packets - keep the LEDs lit up
      identifyModule=10;
      return true;
    }

  case COMMAND::ReadTemperature:
    {
      //Read temperature
      _hardware->ReferenceVoltageOn();
      //Allow reference voltage to stabalize
      delay(5);

      //Return both readings inside the uint16_t
      buffer.moduledata[mymoduleaddress] =TemperatureMeasurement();
      _hardware->ReferenceVoltageOff();
      return true;
    }

  case COMMAND::ReadBadPacketCounter:
    {
      //Report number of bad packets
      buffer.moduledata[mymoduleaddress] = badpackets;
      return true;
    }

    case COMMAND::ReadSettings:
    {
      //Report settings/configuration

      FLOATUNION_t myFloat;
      myFloat.number = _config->LoadResistance;
      buffer.moduledata[0] =myFloat.word[0];
      buffer.moduledata[1] =myFloat.word[1];

      myFloat.number = _config->Calibration;
      buffer.moduledata[2] =myFloat.word[0];
      buffer.moduledata[3] =myFloat.word[1];

      myFloat.number = _config->mVPerADC;
      buffer.moduledata[4] =myFloat.word[0];
      buffer.moduledata[5] =myFloat.word[1];

      buffer.moduledata[6] =_config->BypassOverTempShutdown;
      buffer.moduledata[7] =_config->BypassThresholdmV;
      buffer.moduledata[8] =_config->Internal_BCoefficient;
      buffer.moduledata[9] =_config->External_BCoefficient;

      return true;
    }

    case COMMAND::WriteSettings:
    {
      FLOATUNION_t myFloat;

      myFloat.word[0]=buffer.moduledata[0];
      myFloat.word[1]=buffer.moduledata[1];
      if (myFloat.number<0xFFFF) {
      _config->LoadResistance=myFloat.number;
      }

      myFloat.word[0]=buffer.moduledata[2];
      myFloat.word[1]=buffer.moduledata[3];

      if (myFloat.number<0xFFFF) {
      _config->Calibration=myFloat.number;
      }

      myFloat.word[0]=buffer.moduledata[4];
      myFloat.word[1]=buffer.moduledata[5];
      if (myFloat.number<0xFFFF) {
      _config->mVPerADC=myFloat.number;
      }

      if(buffer.moduledata[6]!=0xFF) {
      _config->BypassOverTempShutdown=buffer.moduledata[6];}

      if(buffer.moduledata[7]!=0xFFFF) {
      _config->BypassThresholdmV=buffer.moduledata[7];
    }
      if(buffer.moduledata[8]!=0xFFFF) {
      _config->Internal_BCoefficient=buffer.moduledata[8];}

        if(buffer.moduledata[9]!=0xFFFF) {
      _config->External_BCoefficient=buffer.moduledata[9];}


      //Save settings
      Settings::WriteConfigToEEPROM((char*)_config, sizeof(CellModuleConfig), EEPROM_CONFIG_ADDRESS);

      return true;
    }
  }

  return false;
}

uint16_t PacketProcessor::TemperatureMeasurement() {
  uint8_t value = TemperatureToByte(Steinhart::ThermistorToCelcius(_config->Internal_BCoefficient, onboard_temperature));

  uint8_t value2 = TemperatureToByte(Steinhart::ThermistorToCelcius(_config->External_BCoefficient, external_temperature));

  return (value << 8) + value2;
}

/*
void PacketProcessor::PrefillRingBuffer() {
  _hardware->ReferenceVoltageOn();

  //Allow the 2.048V reference to stabalize
  delay(100);

  //Sample VCC lots of times so the buffer is full
  for (int i = 0; i < ringsize*2; i++) {
    TakeAnAnalogueReading(ADC_CELL_VOLTAGE);
  }
  _hardware->ReferenceVoltageOff();
}
*/

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

void PacketProcessor::BypassCheck() {
  if (ReadCellVoltageFromBuffer() >= _config->BypassThresholdmV  ) {
      //We need to start bypass
      ByPassEnabled=true;
      return;
  }

  ByPassEnabled=false;
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
  if ((buffer.address & 0x0F) == mymoduleaddress) return true;

  return false;
}

void PacketProcessor::ADCReading(uint16_t value) {
  switch (adcmode) {
  case ADC_CELL_VOLTAGE:
    {
      UpdateRingBuffer(value);
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

bool PacketProcessor::onPacketReceived(const uint8_t * receivebuffer, size_t len) {
  if (len == 0) {
    badpackets++;
    return false;
  }

  // Process your decoded incoming packet here.
  if (len == sizeof(buffer)) {

    //Copy to our buffer (probably a better way to share memory than this)
    memcpy(&buffer, receivebuffer, sizeof(buffer));

    //Calculate the CRC and compare to received
    uint16_t validateCRC = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2);

    if (validateCRC == buffer.crc) {
      //It's a good packet
      if (isPacketForMe()) {
        processPacket();
      }
      return true;
    }

  }

  //We need to do something here, the packet received was not correct
  //Time to do the checks on the cell to make sure everything is okay
  //treat it as if its a watchdog event
  badpackets++;
  return false;
}

uint16_t PacketProcessor::ReadRawRingValue() {
  return ringtotal >> ringsize_bits;
}

//Read cell voltage and return millivolt reading (16 bit unsigned)
uint16_t PacketProcessor::ReadCellVoltageFromBuffer() {
  //TODO: Get rid of the need for float variables?
  float voltage = ((float) ReadRawRingValue() * _config->mVPerADC) * _config->Calibration;

  return (uint16_t) voltage;
}

uint8_t PacketProcessor::TemperatureToByte(float TempInCelcius) {
  //This function reduces the scale of temperatures from float types to a single byte (unsigned)
  //We have an artifical floor at 40oC, anything below 40 is considered negative (below freezing)
  //Gives range of -40 to +216 degrees C

  TempInCelcius += 40;

  //Set the limits
  if (TempInCelcius < 0) TempInCelcius = 0;
  if (TempInCelcius > 255) TempInCelcius = 255;

  return (uint8_t) TempInCelcius;
}

//command byte
// RRRR CCCC
// R    = 4 bits reserved not used
// C    = 4 bits command (16 possible commands)

//commands
// 0000 0000  = identify and provision
// 0000 0001  = read voltage and status
// 0000 0010  = identify module (flash leds)
void PacketProcessor::processPacket() {
  switch (buffer.command & 0x0F) {
  case 0:
    {
      //Set this modules bank address and store in EEPROM
      _config->mybank = buffer.moduledata[mymoduleaddress] & 0x30;
      //Indicate we processed this packet
      buffer.moduledata[mymoduleaddress] = 0xFFFF;
      break;
    }
  case 1:
    {
      //Read voltage of VCC
      if (mymoduleaddress != 0xFF) {
        //Maximum voltage 8191mV
        buffer.moduledata[mymoduleaddress] = ReadCellVoltageFromBuffer() & 0x1FFF;


        //3 top bits remaining
        //X = In bypass
        //Y = Bypass over temperature
        //Z = Not used
      }
      break;
    }
  case 2:
    {
      //identify module (flash leds)
      //TODO: WE NEED TO DO BETTER THAN THIS MOVE TO TIMER/INTERRUPT/FLASHER?

      //Indicate that we received and did something
      buffer.moduledata[mymoduleaddress] = 0xFFFF;

      _hardware->GreenLedOn();
      _hardware->RedLedOn();
      delay(100);
      _hardware->GreenLedOff();
      _hardware->RedLedOff();
      break;
    }

  case 3:
    {
      //Read temperature
      _hardware->ReferenceVoltageOn();
      //Allow reference voltage to stabalize
      delay(5);

      //Internal temperature
      TakeAnAnalogueReading(ADC_INTERNAL_TEMP);
      uint8_t value = TemperatureToByte(Steinhart::ThermistorToCelcius(_config->Internal_BCoefficient, onboard_temperature));

      //External temperature
      TakeAnAnalogueReading(ADC_EXTERNAL_TEMP);
      uint8_t value2 = TemperatureToByte(Steinhart::ThermistorToCelcius(_config->External_BCoefficient, external_temperature));
      _hardware->ReferenceVoltageOff();

      //Return both readings inside the uint16_t
      buffer.moduledata[mymoduleaddress] = (value << 8) + value2;
      break;
    }

  case 4:
    {
      //Report number of bad packets
      buffer.moduledata[mymoduleaddress] = badpackets;
      break;
    }
  }

  buffer.crc = uCRC16Lib::calculate((char * ) & buffer, sizeof(buffer) - 2);
}

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

#include "PacketRequestGenerator.h"


void PacketRequestGenerator::sendSaveSetting(uint8_t b,uint8_t m,uint16_t BypassThresholdmV,uint8_t BypassOverTempShutdown,float LoadResistance,float Calibration,float mVPerADC,uint16_t Internal_BCoefficient,uint16_t External_BCoefficient) {
  setPacketAddress(false,b,m);
  //Command - WriteSettings
  _packetbuffer.command = COMMAND::WriteSettings;

  //Fill packet with 0xFFFF values - module ignores settings
  //with this value
  for ( int a = 0; a < maximum_cell_modules; a++ ) {
    _packetbuffer.moduledata[a] = 0xFFFF;
  }

  // Force refresh of settings
  cmi[b][m].settingsCached = false;

  FLOATUNION_t myFloat;

  myFloat.number=LoadResistance;
  _packetbuffer.moduledata[0]=myFloat.word[0];
  _packetbuffer.moduledata[1]=myFloat.word[1];

  // Arduino float(4 byte)
  myFloat.number=Calibration;
  _packetbuffer.moduledata[2]=myFloat.word[0];
  _packetbuffer.moduledata[3]=myFloat.word[1];

  // Arduino float(4 byte)
  myFloat.number=mVPerADC;
  _packetbuffer.moduledata[4]=myFloat.word[0];
  _packetbuffer.moduledata[5]=myFloat.word[1];

  _packetbuffer.moduledata[6]=BypassOverTempShutdown;
  _packetbuffer.moduledata[7]=BypassThresholdmV;
  _packetbuffer.moduledata[8]=Internal_BCoefficient;
  _packetbuffer.moduledata[9]=External_BCoefficient;

  pushPacketToQueue();
}

void PacketRequestGenerator::sendCellVoltageRequest() {
  //Serial1.println("sendCellVoltageRequest");

  //Read voltage (broadcast) to bank 00
  setPacketAddress(true,0,0);

  //Command 1 - read voltage
  _packetbuffer.command = COMMAND::ReadVoltageAndStatus;

  //AVR MCUs are little endian (least significant byte first in memory)
  clearmoduledata();

  pushPacketToQueue();
}


void PacketRequestGenerator::sendIdentifyModuleRequest(uint8_t b,uint8_t m)
{
  //Serial1.println("sendIdentifyModuleRequest");

  //Read settings from single module
  setPacketAddress(false,b,m);
  //Command 3 - identify
  _packetbuffer.command = COMMAND::Identify;

  //AVR MCUs are little endian (least significant byte first in memory)
  clearmoduledata();

  pushPacketToQueue();

}

void PacketRequestGenerator::sendGetSettingsRequest(uint8_t b,uint8_t m) {
  //Serial1.println("sendGetSettingsRequest");

  //Read settings from single module
  setPacketAddress(false,b,m);
  //Command 5 - read settings
  _packetbuffer.command = COMMAND::ReadSettings;

  //AVR MCUs are little endian (least significant byte first in memory)
  clearmoduledata();

  pushPacketToQueue();
}


void PacketRequestGenerator::sendCellTemperatureRequest() {
  //Serial1.println("sendCellTemperatureRequest");

  //Read voltage (broadcast) to bank 00
  setPacketAddress(true,0,0);
  //Command 3 - read temperatures
  _packetbuffer.command = COMMAND::ReadTemperature;

  //AVR MCUs are little endian (least significant byte first in memory)
  clearmoduledata();

  pushPacketToQueue();
}


void PacketRequestGenerator::pushPacketToQueue() {
  //Serial1.println("pushPacketToQueue");
  //_packetbuffer.crc = uCRC16Lib::calculate((char*)&_packetbuffer, sizeof(packet) - 2);
  _requestq->push(&_packetbuffer);
}

void PacketRequestGenerator::setPacketAddress(bool broadcast,uint8_t bank,uint8_t module) {
  if (broadcast) {
    _packetbuffer.address = B10000000;
  } else {
    _packetbuffer.address = ((bank & B00000011)<<4) + (module & B00001111);
  }
}

void PacketRequestGenerator::clearmoduledata() {
  //todo replace with memset/memclr
  for ( int a = 0; a < maximum_cell_modules; a++ ) {
    _packetbuffer.moduledata[a] = 0;  //__builtin_bswap16(0x0000);
  }
}

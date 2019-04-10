#ifndef DIYBMS_PACKETPROCESSOR_H // include guard
#define DIYBMS_PACKETPROCESSOR_H

#include <Arduino.h>

#include "diybms_attiny841.h"
#include "Steinhart.h"
#include "defines.h"

//Tiny and cross-device compatible CCITT CRC16 calculator library - uCRC16Lib
//https://github.com/Naguissa/uCRC16Lib
#include <uCRC16Lib.h>

#define ADC_CELL_VOLTAGE 0
#define ADC_INTERNAL_TEMP 1
#define ADC_EXTERNAL_TEMP 2

#define maximum_cell_modules 16

struct PacketStruct {
  uint8_t address;
  uint8_t command;
  uint16_t moduledata[maximum_cell_modules];
  uint16_t crc;
}  __attribute__((packed));

typedef union
{
 float number;
 uint8_t bytes[4];
 uint16_t word[2];
} FLOATUNION_t;


class PacketProcessor {
public:
  PacketProcessor(DiyBMSATTiny841* hardware, CellModuleConfig* config) {_hardware=hardware; _config=config;}
  ~PacketProcessor() {}
  void ADCReading(uint16_t value);
  bool onPacketReceived(const uint8_t* receivebuffer, size_t len);
  void PrefillRingBuffer();
  uint16_t ReadCellVoltageFromBuffer();
  byte* GetBufferPointer();
  int GetBufferSize();
  void TakeAnAnalogueReading(uint8_t mode);
  uint16_t ReadRawRingValue();
  uint16_t IncrementWatchdogCounter() { watchdog_counter++; return watchdog_counter; }
  void BypassCheck();
  uint16_t TemperatureMeasurement();
  byte identifyModule;


private:
  DiyBMSATTiny841 *_hardware;
  CellModuleConfig *_config;

  PacketStruct buffer;

  bool processPacket();
  void incrementPacketAddress();
  bool isPacketForMe();
  void UpdateRingBuffer(uint16_t value);
  uint8_t TemperatureToByte(float TempInCelcius);

  #define ringsize_bits 2
  #define ringsize 1 << ringsize_bits

  volatile uint8_t adcmode=0;
  volatile bool ByPassEnabled=false;
  volatile uint16_t ringbuffer[ringsize];
  volatile uint32_t ringtotal=0;
  volatile uint8_t ringptr=0;
  volatile uint16_t onboard_temperature;
  volatile uint16_t external_temperature;

  uint8_t mymoduleaddress = 0xFF;
  uint16_t badpackets = 0;

  volatile uint16_t watchdog_counter=0;

};

#endif

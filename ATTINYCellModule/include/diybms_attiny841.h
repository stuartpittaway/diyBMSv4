#ifndef DIYBMS_ATTINY841_H // include guard
#define DIYBMS_ATTINY841_H

//Show error is not targeting ATTINY841
#if !(defined(__AVR_ATtiny841__))
#error Written for ATTINY841 chip
#endif

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

/*
This class wraps the hardware pins of DIYBMS away from the core logic/code
if you are porting to another chipset, clone this class and modify it.
*/
class DiyBMSATTiny841 {
public:
  DiyBMSATTiny841() {}
  ~DiyBMSATTiny841() {}
  void ConfigurePorts();

   void GreenLedOn();
   void GreenLedOff();

   void DumpLoadOn();
   void DumpLoadOff();


   void SparePinOn();
   void SparePinOff();


   void ReferenceVoltageOn();
   void ReferenceVoltageOff();

   void FlushSerial0();
   void DisableSerial0();
   void EnableSerial0();

   void DisableSerial1();
   void EnableSerial1();

   void EnablePinChangeInterrupt();
   void DisablePinChangeInterrupt();

   void SetWatchdog8sec();

   uint16_t ReadADC();

   void BeginADCReading();

   void Sleep();

   void SelectCellVoltageChannel();
   void SelectInternalTemperatureChannel();
   void SelectExternalTemperatureChannel();
   void EnableStartFrameDetection();

   void DisableSerial0TX();
   void EnableSerial0TX();
   void WaitForSerial0TXFlush();

   void StopTimer2();
   void StartTimer2();
   void SetTimer2Value(uint16_t value);

   void double_tap_green_led();

};

#endif

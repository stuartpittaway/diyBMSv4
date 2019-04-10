/*
 ____  ____  _  _  ____  __  __  ___    _  _  __
(  _ \(_  _)( \/ )(  _ \(  \/  )/ __)  ( \/ )/. |
 )(_) )_)(_  \  /  ) _ < )    ( \__ \   \  /(_  _)
(____/(____) (__) (____/(_/\/\_)(___/    \/   (_)



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

ATTiny841 data sheet
http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8495-8-bit-AVR-Microcontrollers-ATtiny441-ATtiny841_Datasheet.pdf
*/

//If you want to DEBUG connect another serial reading device to RED_LED (TXD1/MISO) this disables the RED LED pin
//#define DIYBMS_DEBUG

#include <Arduino.h>

#if !(F_CPU == 8000000)
#error Processor speed should be 8Mhz internal
#endif

//An Arduino Library that facilitates packet-based serial communication using COBS or SLIP encoding.
//https://github.com/bakercp/PacketSerial
#include <PacketSerial.h>

//96 byte buffer
PacketSerial_<COBS, 0, 100> myPacketSerial;

//Our project code includes
#include "defines.h"
#include "settings.h"

//Default values which get overwritten by EEPROM on power up
CellModuleConfig myConfig;

#include "diybms_attiny841.h"
DiyBMSATTiny841 hardware;

#include "packet_processor.h"
PacketProcessor PP(&hardware,&myConfig);

void DefaultConfig() {
  myConfig.LoadResistance=4.40;
  myConfig.Calibration=2.18000;
  //2mV per ADC resolution
  myConfig.mVPerADC=2048.0/1024.0;
  //Stop running bypass if temperature over 70 degrees C
  myConfig.BypassOverTempShutdown=70;
  myConfig.mybank=0;

  //Start bypass at 4.1 volt
  myConfig.BypassThresholdmV=4100;

  //4150 = B constant (25-50℃)
  myConfig.Internal_BCoefficient=4150;
  //4150 = B constant (25-50℃)
  myConfig.External_BCoefficient=4150;

  // DIYBMS 4 uses the Sunlord SDNT2012X473F4150FTF
  // https://datasheet.lcsc.com/szlcsc/Sunlord-SDNT2012X473F4150FTF_C95969.pdf
  // Manufactor - http://www.sunlordinc.com/searchpn.aspx?se=&mid=32&page=12&nName=98&key=cn
  // Ordering https://lcsc.com/product-detail/NTC-Thermistors_47K-1-B-4150_C95969.html

  // Operating Temp. : -55℃~+125℃
  // Translates as
  // SDNT = NTC Thermistor
  // 2012 = 2012 size (0805 metric 2.0mm×1.25mm)
  // X = N/A
  // 473 = 47,000 ohm zero power resistance (47k nominal)
  // F = +/- 1% tolerance
  // 4150 = B constant (25-50℃)
  // F = 1% Tolerance of B Constant
  // T = Tape & Reel
  // F = N/A

  // Resistance @ 25℃ = 47k, B Constant 4150, 0.20mA max current
  //Using https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html

  //myConfig.Internal.A=0.9676060300E-3;
  //myConfig.Internal.B=2.127893588E-4;
  //myConfig.Internal.C=0.7810451721E-7;
  //myConfig.Internal.A=0.9676060300E-3;
  //myConfig.Internal.B=2.127893588E-4;
  //myConfig.Internal.C=0.7810451721E-7;
}

volatile bool wdt_triggered=false;

ISR(WDT_vect){
  //This is the watchdog timer - something went wrong and no activity recieved in a while
  wdt_triggered=true;
  PP.IncrementWatchdogCounter();

    hardware.GreenLedOff();
    hardware.RedLedOff();

  }

void double_tap_green_led() {
  hardware.GreenLedOn();
  delay(60);
  hardware.GreenLedOff();
  delay(60);
  hardware.GreenLedOn();
  delay(60);
  hardware.GreenLedOff();
}

ISR(ADC_vect)
{
  // when ADC completed, take an interrupt and process result
  uint16_t value=hardware.ReadADC();
  PP.ADCReading(value);
}

void BeginSetupProcedure()
{
  //This should be called AFTER PrefillRingBuffer has already ran

  //Assume that we have just been programmed via ISP header so VCC is now at 3.3 volts (approx)
  //Configure our ADC readings accordingly (will need fine tuning later on)

  //Determine ADC value (averaged)
  uint16_t raw=PP.ReadRawRingValue();

  //3300 millivolt should be the voltage from our ISP programmer
  //in reality it won't be this accurate but is a reasonable starting point
  myConfig.Calibration = 3300.0 / ((float)raw * myConfig.mVPerADC);

  #ifdef DIYBMS_DEBUG
  Serial1.print(raw);
  Serial1.print(F("=3.3V Calib="));
  Serial1.println(myConfig.Calibration,6);
  #endif

  //Save settings
  Settings::WriteConfigToEEPROM((char*)&myConfig, sizeof(myConfig), EEPROM_CONFIG_ADDRESS);

  //We leave the reference voltages enabled so they can be probed with a multimeter if needed at this point
  hardware.ReferenceVoltageOn();

  //Flash LED to indicate we are finished and wait for a power cycle
  while(1) {
    wdt_reset();

    #ifdef DIYBMS_DEBUG
    PP.TakeAnAnalogueReading(ADC_CELL_VOLTAGE);
    Serial1.println(PP.ReadCellVoltageFromBuffer());
    #endif

    double_tap_green_led();
    delay(2000);
  }
}

void onPacketReceived(const uint8_t* receivebuffer, size_t len) {

    if (len>0) {

      //A data packet has just arrived, process it and forward the results to the next module (if valid)
      if (PP.onPacketReceived(receivebuffer,len)) {

      }

      //Wake up the connected cell module from sleep
      Serial.write((byte)0);
      delay(10);

      //Send the packet (even if it was invalid so controller can count crc errors)
      myPacketSerial.send(PP.GetBufferPointer(), PP.GetBufferSize());
    }

    hardware.GreenLedOff();
}


ISR (USART0_START_vect) {
  //Needs to be here!
  asm("NOP");

  hardware.GreenLedOn();
}

void setup() {
  //Must be first line of code
  wdt_disable();

  //8 second between watchdogs
  hardware.SetWatchdog8sec();

  //Setup IO ports
  hardware.ConfigurePorts();

  //More power saving changes
  hardware.EnableSerial0();


  #ifdef DIYBMS_DEBUG
  //UCSR1B |=(1<<TXEN1); // enable TX Serial1 for DEBUG output
  Serial1.begin(38400,SERIAL_8N1);
  DEBUG_PRINT(F("\r\nDEBUG MODE"))
  #else
  hardware.DisableSerial1();
  #endif

  //Check if setup routine needs to be run
  if (!Settings::ReadConfigFromEEPROM((char*)&myConfig, sizeof(myConfig),EEPROM_CONFIG_ADDRESS)) {
    hardware.GreenLedOn();
    DefaultConfig();
    PP.PrefillRingBuffer();
    hardware.GreenLedOff();

    BeginSetupProcedure();
  }

  //Normal start up
  PP.PrefillRingBuffer();

  double_tap_green_led();

  //Set up data handler
  Serial.begin(4800, SERIAL_8N1);
  myPacketSerial.setStream(&Serial);           // start serial for output
  myPacketSerial.setPacketHandler(&onPacketReceived);
}


void loop() {
  wdt_reset();

  if (PP.identifyModule>0) {
    hardware.RedLedOn();
    hardware.GreenLedOn();
  }


  if (PP.identifyModule==0) {
    //We dont sleep if we are in identify mode

    hardware.EnableStartFrameDetection();

    UCSR0B &= ~_BV(TXEN0);  //disable transmitter (saves 6mA)

    //Program stops here until woken by watchdog or pin change interrupt
    hardware.Sleep();
  }


  if (wdt_triggered) {
    //Put RED LED on whilst we sample after a watchdog event
    hardware.RedLedOn();
  }

  //We always take a voltage reading on every loop cycle to check if we need to go into bypass
  //this is also triggered by the watchdog should comms fail or the module is running standalone
  hardware.ReferenceVoltageOn();
  //allow 2V to stabalize
  delay(10);
  PP.TakeAnAnalogueReading(ADC_CELL_VOLTAGE);

  if (wdt_triggered) {
    //If watchdog then check the temperature as well
    uint16_t temperatures=PP.TemperatureMeasurement();
  }

  PP.BypassCheck();

  hardware.ReferenceVoltageOff();

  if (wdt_triggered) {
    hardware.RedLedOff();
    wdt_triggered=false;
  }


  //We may have got here because the watchdog (8seconds) went off - we didnt receive a packet of data
  if (!wdt_triggered) {
    UCSR0B |=(1<<TXEN0); // enable TX Serial0

    //Loop here processing any packets then go back to sleep
    for (size_t i = 0; i <5; i++) {
      //Allow data to be received in buffer
      delay(10);
      // Call update to receive, decode and process incoming packets.
      myPacketSerial.update();
    }
  }

  if (PP.identifyModule>0) {
    hardware.RedLedOff();
    hardware.GreenLedOff();
    delay(50);
    PP.identifyModule--;
  }

}

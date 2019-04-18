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
PacketSerial_<COBS, 0, 64> myPacketSerial;

//Our project code includes
#include "defines.h"
#include "settings.h"

#include <PID_v1.h>

//Default values which get overwritten by EEPROM on power up
CellModuleConfig myConfig;

#include "diybms_attiny841.h"
DiyBMSATTiny841 hardware;

#include "packet_processor.h"
PacketProcessor PP(&hardware,&myConfig);

volatile bool wdt_triggered=false;

void DefaultConfig() {
  myConfig.LoadResistance=4.40;

  //About 2.2100 seems about right
  myConfig.Calibration=2.21000;

  //2mV per ADC resolution
  myConfig.mVPerADC=2.0;  //2048.0/1024.0;

  //Stop running bypass if temperature over 70 degrees C
  myConfig.BypassOverTempShutdown=70;

  myConfig.mybank=0;

  //Start bypass at 4.1 volt
  myConfig.BypassThresholdmV=4100;

  //4150 = B constant (25-50℃)
  myConfig.Internal_BCoefficient=4150;
  //4150 = B constant (25-50℃)
  myConfig.External_BCoefficient=4150;

  // Resistance @ 25℃ = 47k, B Constant 4150, 0.20mA max current
  //Using https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
}


ISR(WDT_vect){
  //This is the watchdog timer - something went wrong and no activity recieved in a while
  wdt_triggered=true;
  PP.IncrementWatchdogCounter();
}


ISR(ADC_vect)
{
  // when ADC completed, take an interrupt and process result
  PP.ADCReading(hardware.ReadADC());
}

void BeginSetupProcedure()
{
  //Assume that we have just been programmed via ISP header so VCC is now at 3.3 volts (approx)
  //Configure our ADC readings accordingly (will need fine tuning later on)

  hardware.ReferenceVoltageOn();

  //Allow the 2.048V reference to stabalize
  delay(100);

  //Determine ADC value
  for (size_t i = 0; i < 32; i++) {
    PP.TakeAnAnalogueReading(ADC_CELL_VOLTAGE);
  }

  uint16_t raw=PP.RawADCValue();

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

  //Flash LED to indicate we are finished and wait for a power cycle
  while(1) {
    wdt_reset();

    #ifdef DIYBMS_DEBUG
    PP.TakeAnAnalogueReading(ADC_CELL_VOLTAGE);
    Serial1.println(PP.ReadCellVoltageFromBuffer());
    #endif

    hardware.double_tap_green_led();
    delay(2000);
  }
}

void onPacketReceived(const uint8_t* receivebuffer, size_t len) {

    if (len>0) {

      //A data packet has just arrived, process it and forward the results to the next module
      PP.onPacketReceived(receivebuffer,len);

      hardware.EnableSerial0TX();

      //Wake up the connected cell module from sleep
      Serial.write((byte)0);
      delay(10);


      //Send the packet (even if it was invalid so controller can count crc errors)
      myPacketSerial.send(PP.GetBufferPointer(), PP.GetBufferSize());

      hardware.WaitForSerial0TXFlush();
      hardware.DisableSerial0TX();
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
    DefaultConfig();
    BeginSetupProcedure();
  }

  hardware.double_tap_green_led();
  hardware.double_tap_red_led();

  //Set up data handler
  Serial.begin(4800, SERIAL_8N1);

  myPacketSerial.setStream(&Serial);
  myPacketSerial.setPacketHandler(&onPacketReceived);
}


uint16_t bypassCountDown=0;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
//double Kp=2, Ki=5, Kd=1;
//P_ON_M specifies that Proportional on Measurement be used
//P_ON_E (Proportional on Error) is the default behavior

//Kp: Determines how aggressively the PID reacts to the current amount of error (Proportional) (double >=0)
//Ki: Determines how aggressively the PID reacts to error over time (Integral) (double>=0)
//Kd: Determines how aggressively the PID reacts to the change in error (Derivative) (double>=0)
PID myPID(&Input, &Output, &Setpoint, 20, 6, 4,P_ON_E, DIRECT);

uint8_t bypassHasJustFinished=0;

void loop() {
  wdt_reset();

  if (PP.identifyModule>0) {
    hardware.GreenLedOn();
    PP.identifyModule--;

    if (PP.identifyModule==0) {
      hardware.GreenLedOff();
    }
  }

  if (!PP.WeAreInBypass && bypassHasJustFinished==0) {
    //We don't sleep if we are in bypass mode or just after completing bypass
    hardware.EnableStartFrameDetection();

    //Program stops here until woken by watchdog or pin change interrupt
    hardware.Sleep();
  }

  //We are awake....

  if (wdt_triggered) {
    //Flash green LED twice after a watchdog wake up
    hardware.double_tap_green_led();
  }

  //We always take a voltage and temperature reading on every loop cycle to check if we need to go into bypass
  //this is also triggered by the watchdog should comms fail or the module is running standalone
  //Probably over kill to do it this frequently
  hardware.ReferenceVoltageOn();

  //allow 2.048V to stabalize
  delay(10);

  PP.TakeAnAnalogueReading(ADC_CELL_VOLTAGE);
  //Internal temperature
  PP.TakeAnAnalogueReading(ADC_INTERNAL_TEMP);
  //External temperature
  PP.TakeAnAnalogueReading(ADC_EXTERNAL_TEMP);

  hardware.ReferenceVoltageOff();

  if (PP.BypassCheck()) {
      //Our cell voltage is OVER the setpoint limit, start draining cell using load bypass resistor

      if (!PP.WeAreInBypass) {
        //We have just entered the bypass code

        //We want the PID to keep at this temperature
        Setpoint=myConfig.BypassOverTempShutdown;

        //The TIMER2 can vary between 0 and 10,000
        myPID.SetOutputLimits(0, 10000);

        //Start timer2 with zero value
        hardware.StartTimer2();

        //Run the PID
        myPID.SetMode(AUTOMATIC);
        PP.WeAreInBypass=true;

        //This controls how many cycles of loop() we make before re-checking the situation
        bypassCountDown=200;
      }
  }

  if (bypassCountDown>0) {
    //Compare the real temperature against max setpoint
    Input = PP.InternalTemperature();
    if (myPID.Compute()) {
        //Change Timer2 if needed
        hardware.SetTimer2Value(Output);
    }

    bypassCountDown--;

    if (bypassCountDown==0) {
      //Switch everything off for this cycle

      PP.WeAreInBypass=false;

      myPID.SetMode(MANUAL);
      hardware.StopTimer2();

      //Just to be sure, switch everything off
      hardware.RedLedOff();
      hardware.DumpLoadOff();

      //On the next iteration of loop, don't sleep so we are forced to take another
      //cell voltage reading without the bypass being enabled, and we can then
      //evaludate if we need to stay in bypass mode, we do this a few times
      //as the cell has a tendancy to float back up in voltage once load resistor is removed
      bypassHasJustFinished=200;
    }
  }

  if (wdt_triggered) {
    //We got here because the watchdog (after 8 seconds) went off - we didnt receive a packet of data
    wdt_triggered=false;
  } else
  {
    //Loop here processing any packets then go back to sleep

    //NOTE this loop size is dependant on the size of the packet buffer (34 bytes)
    //     too small a loop will prevent anything being processed as we go back to Sleep
    //     before packet is received correctly
    for (size_t i = 0; i <15; i++) {
      //Allow data to be received in buffer
      delay(10);

      // Call update to receive, decode and process incoming packets.
      myPacketSerial.update();
    }
  }

  if (bypassHasJustFinished>0) { bypassHasJustFinished--; }
}

/*

 ____  ____  _  _  ____  __  __  ___    _  _  __
(  _ \(_  _)( \/ )(  _ \(  \/  )/ __)  ( \/ )/. |
 )(_) )_)(_  \  /  ) _ < )    ( \__ \   \  /(_  _)
(____/(____) (__) (____/(_/\/\_)(___/    \/   (_)

  (c) 2017/18/19 Stuart Pittaway

  This is the code for the controller - it talks to the V4 cell modules over isolated serial bus

  This code runs on ESP-8266-12E (NODE MCU 1.0) and compiles with Arduino 1.8.5 environment

  Arduino settings
  NodeMCU 1.0 (ESP-12E module), Flash 4M (3MSPIFF), CPU 80MHZ

  Setting up ESP-8266-12E (NODE MCU 1.0) on Arduino
  http://www.instructables.com/id/Programming-a-HTTP-Server-on-ESP-8266-12E/

  "c:\Program Files (x86)\PuTTY\putty.exe" -serial COM4 -sercfg 115200,8,n,1,N
*/

/*
   PINS
   D6 = GREEN_LED
   D7 = GPIO13 = RECEIVE SERIAL
   D8 = GPIO15 = TRANSMIT SERIAL

   D4 = GPIO2 = TXD1 = TRANSMIT DEBUG SERIAL (and blue led on esp8266)

   DIAGRAM
   https://www.hackster.io/Aritro/getting-started-with-esp-nodemcu-using-arduinoide-aa7267
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
//#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PacketSerial.h>
//Tiny and cross-device compatible CCITT CRC16 calculator library - uCRC16Lib
//https://github.com/Naguissa/uCRC16Lib
#include <uCRC16Lib.h>

AsyncWebServer server(80);

#include "PacketRequestGenerator.h"
#include "defines.h"

void dumpPacketToDebug(packet *buffer);

packet buffer;
CellModuleInfo cmi[4][maximum_cell_modules];
int numberOfModules[4];

#include <cppQueue.h>

#include "settings.h"
#include "SoftAP.h"
#include "DIYBMSServer.h"

// Instantiate queue
Queue	requestQueue(sizeof(packet), 25, FIFO);

PacketRequestGenerator prg=PacketRequestGenerator(&requestQueue);

PacketSerial_<COBS, 0, 256> myPacketSerial;

os_timer_t myTimer;
os_timer_t myTransmitTimer;

bool waitingForReply=false;
uint8_t missedPacketCount=0;

uint16_t totalMissedPacketCount=0;
uint16_t totalCRCErrors=0;
uint16_t totalNotProcessedErrors=0;

uint8_t ReplyFromBank() {return (buffer.address & B00110000) >> 4;}
uint8_t ReplyLastAddress() {return buffer.address & 0x0F;}

bool ReplyWasProcessedByAModule() {return (buffer.command & B10000000)>0;}

bool requestPending;

void ProcessReplyAddressByte() {
  //address byte
  // B X KK AAAA
  // B    = 1 bit broadcast to all modules (in the bank)
  // X    = 1 bit unused
  // KK   = 2 bits module bank select (4 possible banks of 16 modules) - reserved and not used
  // AAAA = 4 bits for address (module id 0 to 15)

  uint8_t broadcast=(buffer.address & B10000000) >> 7;
  //uint8_t bank=(buffer.address & B00110000) >> 4;
  //uint8_t lastAddress=buffer.address & 0x0F;

  //Only set if it was a reply from a broadcast message
  if (broadcast>0) {
    if (numberOfModules[ReplyFromBank()]!=ReplyLastAddress()) {
        numberOfModules[ReplyFromBank()]=ReplyLastAddress();

        //if we have a different number of modules in this bank
        //we should clear all the cached config flags from the modules
        //as they have probably moved address
        for (size_t i = 0; i < maximum_cell_modules; i++)
        {
          cmi[ReplyFromBank()][i].settingsCached=false;
          cmi[ReplyFromBank()][i].voltagemVMin=6000;
          cmi[ReplyFromBank()][i].voltagemVMax=0;
        }
    }
  }
}

void ProcessReplyTemperature() {
  //Called when a decoded packet has arrived in buffer for command 3

  ProcessReplyAddressByte();
  //40 offset for below zero temps
  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    cmi[ReplyFromBank()][i].internalTemp = ((buffer.moduledata[i] & 0xFF00)>>8)-40;
    cmi[ReplyFromBank()][i].externalTemp= (buffer.moduledata[i] & 0x00FF)-40;
  }
}


void ProcessReplyVoltage() {
  //Called when a decoded packet has arrived in buffer for command 1

  ProcessReplyAddressByte();

  uint8_t b=ReplyFromBank();

  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    cmi[b][i].voltagemV = buffer.moduledata[i] & 0x1FFF;
    cmi[b][i].inBypass= (buffer.moduledata[i] & 0x8000)>0;
    cmi[b][i].bypassOverTemp= (buffer.moduledata[i] & 0x4000)>0;

    if (cmi[b][i].voltagemV> cmi[b][i].voltagemVMax) {
      cmi[b][i].voltagemVMax=cmi[b][i].voltagemV;
    }

    if (cmi[b][i].voltagemV<cmi[b][i].voltagemVMin) {
      cmi[b][i].voltagemVMin=cmi[b][i].voltagemV;
    }
  }

  //3 top bits remaining
  //X = In bypass
  //Y = Bypass over temperature
  //Z = Not used
}

void ProcessReplySettings() {
  uint8_t b=ReplyFromBank();
  uint8_t m=ReplyLastAddress();

  //TODO Validate b and m here to prevent array overflow
  cmi[b][m].settingsCached=true;
  cmi[b][m].settingsRequested=false;

  cmi[b][m].BypassOverTempShutdown=buffer.moduledata[3] & 0x00FF;
  cmi[b][m].BypassThresholdmV=buffer.moduledata[4];
  cmi[b][m].LoadResistance=buffer.moduledata[0];
  cmi[b][m].Calibration=buffer.moduledata[1];
  cmi[b][m].mVPerADC=buffer.moduledata[2];
  cmi[b][m].Internal_BCoefficient=buffer.moduledata[5];
  cmi[b][m].External_BCoefficient=buffer.moduledata[6];
}

void onPacketReceived(const uint8_t* receivebuffer, size_t len)
{
  // Process your decoded incoming packet here.
  //Serial1.print("PR=");
  //Serial1.print(len);
  //Serial1.print(" bytes =");

  Serial1.print("Recv:");

  if (len > 0) {

    //Copy to our buffer (probably a better way to share memory than this)
    memcpy(&buffer, receivebuffer, sizeof(buffer));

    //Calculate the CRC and compare to received
    uint16_t validateCRC = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2) ;

    dumpPacketToDebug(&buffer);
    Serial1.print('=');

    if (validateCRC==buffer.crc) {
        Serial1.print("good");

        //Subtract good packets from total sent count
        //totalMissedPacketCount should be near zero as possible
        totalMissedPacketCount--;

        if (ReplyWasProcessedByAModule()) {

        switch (buffer.command & 0x0F) {
          case 0: break;
          case 1: ProcessReplyVoltage();          break;
          case 2: break;  //Ignore reply
          case 3: ProcessReplyTemperature();          break;
          case 4: break;
          case 5: ProcessReplySettings();          break;
        }
      } else {
        totalNotProcessedErrors++;
        Serial1.print("request ignored");
      }

    } else {
      Serial1.print("bad");
      totalCRCErrors++;
    }

    waitingForReply=false;
  }

  Serial1.println("");
}

void timerTransmitCallback(void *pArg) {
  //Called every second to transmit anything that remains in the queue
  if (!waitingForReply && !requestQueue.isEmpty()) {
    GREEN_LED_ON;

    //Wake up the connected cell module from sleep
    Serial.write(0x00);
    delay(10);

    packet transmitBuffer;

    requestQueue.pop(&transmitBuffer);

    myPacketSerial.send((byte*)&transmitBuffer, sizeof(transmitBuffer));

    //Increase the count of sent packets
    totalMissedPacketCount++;

    Serial1.print("Send:");
    dumpPacketToDebug(&transmitBuffer);
    Serial1.println("");

    //waitingForReply=true;
    missedPacketCount=0;
    GREEN_LED_OFF;

    Serial1.print("Q depth:");
    Serial1.println(requestQueue.getCount());
  }
}

uint8_t packetType=0;

void timerCallback(void *pArg) {
  //this is called regularly on a timer, it determines what request to make
  //to the modules (via the request queue)

  packetType++;

  prg.sendCellVoltageRequest();

  if (packetType==3) {
    //Every few packets also get the temperature
    prg.sendCellTemperatureRequest();
    packetType=0;
  }
}

//command byte
// WRRR CCCC
// W    = 1 bit controller send (0) module processed (1)
// R    = 3 bits reserved not used
// C    = 4 bits command (16 possible commands)

//commands
// 1000 0000  = identify and provision
// 0000 0001  = read voltage and status
// 0000 0010  = identify module (flash leds)
// 0000 0011  = Read temperature
// 0000 0100  = Report number of bad packets
// 0000 0101  = Report settings/configuration

void dumpPacketToDebug(packet *buffer) {
  Serial1.print(buffer->address,HEX);
  Serial1.print(' ');
  Serial1.print(buffer->command,HEX);
  Serial1.print(' ');
  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    Serial1.print(buffer->moduledata[i],HEX);
    Serial1.print(' ');
  }
  Serial1.print(buffer->crc,HEX);
}



void setup() {
  //Serial is used for communication to modules, Serial1 is for debug output

  pinMode(GREEN_LED, OUTPUT);
  GREEN_LED_OFF;

  numberOfModules[0]=0;
  numberOfModules[1]=0;
  numberOfModules[2]=0;
  numberOfModules[3]=0;

  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    cmi[0][i].voltagemVMax=0;
    cmi[0][i].voltagemVMin=6000;
    cmi[1][i].voltagemVMax=0;
    cmi[1][i].voltagemVMin=6000;
    cmi[2][i].voltagemVMax=0;
    cmi[2][i].voltagemVMin=6000;
    cmi[3][i].voltagemVMax=0;
    cmi[3][i].voltagemVMin=6000;
  }

  Serial.begin(4800, SERIAL_8N1);           // Serial for comms to modules
  //Use alternative GPIO pins of D7/D8
  //D7 = GPIO13 = RECEIVE SERIAL
  //D8 = GPIO15 = TRANSMIT SERIAL
  Serial.swap();

  myPacketSerial.setStream(&Serial);           // start serial for output
  myPacketSerial.setPacketHandler(&onPacketReceived);

  //Debug serial output
  Serial1.begin(115200, SERIAL_8N1);
  Serial1.setDebugOutput(true);

  //Ensure we service the cell modules every 2 seconds
  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, 4000, true);

  os_timer_setfn(&myTransmitTimer, timerTransmitCallback, NULL);
  os_timer_arm(&myTransmitTimer, 1000, true);

  //D0 is used to reset access point WIFI details on boot up
  pinMode(D0,INPUT_PULLUP);

  //This is normally pulled high
  uint8_t clearAPSettings=digitalRead(D0);

  //Also need to check here for a button being pressed to reconfigure ESP8266
  if (!DIYBMSSoftAP::LoadConfigFromEEPROM() || clearAPSettings==0) {
      Serial1.println("SetupAccessPoint");
      //We are in initial power on mode (factory reset)
      DIYBMSSoftAP::SetupAccessPoint(&server);
  } else {
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
      would try to act as both a client and an access-point and could cause
      network-issues with your other WiFi-devices on your WiFi-network. */

      Serial1.println(F("Connecting to WIFI"));

      WiFi.mode(WIFI_STA);
      WiFi.begin(DIYBMSSoftAP::WifiSSID(), DIYBMSSoftAP::WifiPassword());
  }

}

bool server_running=false;

void loop() {
  // Call update to receive, decode and process incoming packets.
  if (Serial.available()) {
    myPacketSerial.update();
  }

/*
TODO: CHECK ERROR CODES BETTER!
0 : WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
1 : WL_NO_SSID_AVAIL in case configured SSID cannot be reached
3 : WL_CONNECTED after successful connection is established
4 : WL_CONNECT_FAILED if password is incorrect
6 : WL_DISCONNECTED if module is not configured in station mode
*/
  if (WiFi.status() == WL_CONNECTED && !server_running)
  {
    Serial1.print( WiFi.status() );
    Serial1.print(F(". Connected IP:"));
    Serial1.println(WiFi.localIP());

    DIYBMSServer::StartServer(&server);
    server_running=true;
  }

  if (server_running)
  {
    //Do something clever here
  }

}

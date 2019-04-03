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

   D7 = GPIO13 = RECEIVE SERIAL
   D8 = GPIO15 = TRANSMIT SERIAL

   D4 = GPIO2 = TXD1 = TRANSMIT DEBUG SERIAL

   DIAGRAM
   https://www.hackster.io/Aritro/getting-started-with-esp-nodemcu-using-arduinoide-aa7267
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PacketSerial.h>
//Tiny and cross-device compatible CCITT CRC16 calculator library - uCRC16Lib
//https://github.com/Naguissa/uCRC16Lib
#include <uCRC16Lib.h>

AsyncWebServer server(80);

#include "defines.h"

packet buffer;
CellModuleInfo cmi[4][maximum_cell_modules];
int numberOfModules[4];

#include "settings.h"
#include "SoftAP.h"
#include "DIYBMSServer.h"

PacketSerial_<COBS, 0, 256> myPacketSerial;

os_timer_t myTimer;

void sendCellVoltageRequest();
void sendCellTemperatureRequest();

bool waitingForReply=false;
uint8_t missedPacketCount=0;

uint16_t totalMissedPacketCount=0;
uint16_t totalCRCErrors=0;

uint8_t ReplyFromBank() {return (buffer.address & B00110000) >> 4;}
uint8_t ReplyLastAddress() {return buffer.address & 0x0F;}

void ProcessReplyAddressByte() {
  //address byte
  // B X KK AAAA
  // B    = 1 bit broadcast to all modules (in the bank)
  // X    = 1 bit unused
  // KK   = 2 bits module bank select (4 possible banks of 16 modules) - reserved and not used
  // AAAA = 4 bits for address (module id 0 to 15)

  //uint8_t broadcast=(buffer.address & B10000000) >> 7;
  //uint8_t bank=(buffer.address & B00110000) >> 4;
  //uint8_t lastAddress=buffer.address & 0x0F;

  numberOfModules[ReplyFromBank()]=ReplyLastAddress();
}

void ProcessReplyTemperature() {
  //Called when a decoded packet has arrived in buffer for command 3

  ProcessReplyAddressByte();

  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    cmi[ReplyFromBank()][i].internalTemp = ((buffer.moduledata[i] & 0xFF00)>>8)-40;
    cmi[ReplyFromBank()][i].externalTemp= (buffer.moduledata[i] & 0x00FF)-40;
  }
}


void ProcessReplyVoltage() {
  //Called when a decoded packet has arrived in buffer for command 1

  ProcessReplyAddressByte();

  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    cmi[ReplyFromBank()][i].voltagemV = buffer.moduledata[i] & 0x1FFF;
    cmi[ReplyFromBank()][i].inBypass= (buffer.moduledata[i] & 0x8000)>0;
    cmi[ReplyFromBank()][i].bypassOverTemp= (buffer.moduledata[i] & 0x4000)>0;

    if (cmi[ReplyFromBank()][i].voltagemV> cmi[ReplyFromBank()][i].voltagemVMax) {
      cmi[ReplyFromBank()][i].voltagemVMax=cmi[ReplyFromBank()][i].voltagemV;
    }

    if (cmi[ReplyFromBank()][i].voltagemV<cmi[ReplyFromBank()][i].voltagemVMin) {
      cmi[ReplyFromBank()][i].voltagemVMin=cmi[ReplyFromBank()][i].voltagemV;
    }

  }

  //3 top bits remaining
  //X = In bypass
  //Y = Bypass over temperature
  //Z = Not used
}


void onPacketReceived(const uint8_t* receivebuffer, size_t len)
{
  // Process your decoded incoming packet here.
  Serial1.print("PR=");
  Serial1.print(len);
  Serial1.print(" bytes =");

  if (len > 0) {

    //Copy to our buffer (probably a better way to share memory than this)
    memcpy(&buffer, receivebuffer, sizeof(buffer));

    //Calculate the CRC and compare to received
    uint16_t validateCRC = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2) ;

    Serial1.print(buffer.address,HEX);
    Serial1.print(' ');
    Serial1.print(buffer.command,HEX);
    Serial1.print(' ');
    for (size_t i = 0; i < maximum_cell_modules; i++)
    {
      Serial1.print(buffer.moduledata[i],HEX);
      Serial1.print(' ');
    }
    Serial1.print(buffer.crc,HEX);
    Serial1.print('=');

    if (validateCRC==buffer.crc) {
        Serial1.print("good");

        switch (buffer.command & 0x0F) {
          case 0:
          break;
          case 1: ProcessReplyVoltage();
          break;
          case 3: ProcessReplyTemperature();
          break;
        }
    } else {
      Serial1.print("bad");
      totalCRCErrors++;
    }

    waitingForReply=false;
  }

  Serial1.print(' ');
  Serial1.print(totalCRCErrors);
  Serial1.print(' ');
  Serial1.print(missedPacketCount);
  Serial1.println("*");
}


uint8_t requestPacketType=0;

void timerCallback(void *pArg) {
  if (!waitingForReply) {
    GREEN_LED_ON;

    requestPacketType++;

    //Wake up the connected cell module from sleep
    Serial.write(0x00);
    delay(10);

    if (requestPacketType<5) {
      Serial1.println("Sending voltage request");
      sendCellVoltageRequest();
    } else {
      Serial1.println("Sending temp request");
      sendCellTemperatureRequest();
      requestPacketType=0;
    }

    GREEN_LED_OFF;
    waitingForReply=true;
    missedPacketCount=0;

    //Ensure we are empty for next reply
    //Serial.flush();

  } else {
    missedPacketCount++;
    totalMissedPacketCount++;
    Serial1.print('W');
    Serial1.print(missedPacketCount);
    if (missedPacketCount>2) {
      //We didnt receive a reply to a packet we sent previously something went wrong
      //raise an error here and recover
      waitingForReply=false;
      //missedPacketCount=0;
    }
  }

}

void clearmoduledata() {
  for ( int a = 0; a < maximum_cell_modules; a++ ) {
    buffer.moduledata[a] = __builtin_bswap16(0x0000);
  }
}


//command byte
// WRRR CCCC
// W    = 1 bit read (0) or write (1)
// R    = 3 bits reserved not used
// C    = 4 bits command (16 possible commands)

//commands
// 1000 0000  = identify and provision
// 0000 0001  = read voltage and status

void sendCellVoltageRequest() {
  //Read voltage (broadcast) to bank 00
  buffer.address = B10000000;
  //Command 1 - read voltage
  buffer.command = B00000001;

  //AVR MCUs are little endian (least significant byte first in memory)
  clearmoduledata();

  buffer.crc = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2);
  myPacketSerial.send((byte*)&buffer, sizeof(buffer));
}

void sendCellTemperatureRequest() {
  //Read voltage (broadcast) to bank 00
  buffer.address = B10000000;
  //Command 3 - read temperatures
  buffer.command = B00000011;

  //AVR MCUs are little endian (least significant byte first in memory)
  clearmoduledata();

  buffer.crc = uCRC16Lib::calculate((char*)&buffer, sizeof(buffer) - 2);
  myPacketSerial.send((byte*)&buffer, sizeof(buffer));
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
  os_timer_arm(&myTimer, 2000, true);

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

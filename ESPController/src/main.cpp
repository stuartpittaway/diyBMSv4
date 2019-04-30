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
#include <ESPAsyncWebServer.h>
#include <AsyncMqttClient.h>
#include <PacketSerial.h>
#include <cppQueue.h>
#include <Ticker.h>

#include "defines.h"

AsyncWebServer server(80);

bool server_running=false;

uint8_t packetType=0;

//This large array holds all the information about the modules
//up to 4x16
CellModuleInfo cmi[4][maximum_cell_modules];
//Size of array
int numberOfModules[4];

#include "settings.h"
#include "SoftAP.h"
#include "DIYBMSServer.h"
#include "PacketRequestGenerator.h"
#include "PacketReceiveProcessor.h"

// Instantiate queue to hold packets ready for transmission
Queue	requestQueue(sizeof(packet), 16, FIFO);

PacketRequestGenerator prg=PacketRequestGenerator(&requestQueue);

PacketReceiveProcessor receiveProc=PacketReceiveProcessor();

PacketSerial_<COBS, 0, 128> myPacketSerial;

volatile bool waitingForReply=false;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

Ticker myTimer;
Ticker myTransmitTimer;
Ticker wifiReconnectTimer;
Ticker mqttReconnectTimer;
Ticker myTimerSendMqttPacket;

uint16_t sequence=0;

AsyncMqttClient mqttClient;

void dumpPacketToDebug(packet *buffer) {
  Serial1.print(buffer->address,HEX);
  Serial1.print('/');
  Serial1.print(buffer->command,HEX);
  Serial1.print('/');
  Serial1.print(buffer->sequence,HEX);
  Serial1.print('=');
  for (size_t i = 0; i < maximum_cell_modules; i++)
  {
    Serial1.print(buffer->moduledata[i],HEX);
    Serial1.print(" ");
  }
  Serial1.print(" =");
  Serial1.print(buffer->crc,HEX);
}


void onPacketReceived(const uint8_t* receivebuffer, size_t len)
{
  //Note that this function gets called frequently with zero length packets
  //due to the way the modules operate

  if (len==sizeof(packet)) {
    // Process decoded incoming packet
    Serial1.print("Recv:");

    dumpPacketToDebug((packet*)receivebuffer);

    if (!receiveProc.ProcessReply(receivebuffer,sequence)) {
      Serial1.println("** FAILED PROCESS REPLY **");
    }

    //We received a packet (although may have been an error)
    waitingForReply=false;

    Serial1.println();
  }

}


void timerTransmitCallback() {

  //Don't send another message until we have received reply from the last one
  //this slows the transmit process down a lot so potentially need to look at a better
  //way to do this and also keep track of mising messages replies for error tracking
  if (waitingForReply) {

    //Serial1.print('E');Serial1.print(receiveProc.commsError);

    //Increment the counter to watch for complete comms failures
    receiveProc.commsError++;

    //After 5 attempts give up and send another packet
    if (receiveProc.commsError>10) {
      waitingForReply=false;
      receiveProc.totalMissedPacketCount++;
    } else {
        return;
    }
  }

  //Called every second to transmit anything that remains in the transmit queue
  if (!requestQueue.isEmpty()) {
    packet transmitBuffer;

    GREEN_LED_ON;

    //Wake up the connected cell module from sleep
    Serial.write(0x00);
    delay(10);

    requestQueue.pop(&transmitBuffer);

    sequence++;

    transmitBuffer.sequence=sequence;
    transmitBuffer.crc = uCRC16Lib::calculate((char*)&transmitBuffer, sizeof(packet) - 2);

    myPacketSerial.send((byte*)&transmitBuffer, sizeof(transmitBuffer));

    waitingForReply=true;

    Serial1.print("Send:");
    dumpPacketToDebug(&transmitBuffer);

    Serial1.print("/Q:");
    Serial1.print(requestQueue.getCount());
    Serial1.print(" # ");

    GREEN_LED_OFF;


  }
}


void timerEnqueueCallback() {
  //this is called regularly on a timer, it determines what request to make to the modules (via the request queue)
  //packetType++;

  prg.sendCellVoltageRequest();

  //if (packetType==3) {
    //Every few packets also get the temperature
    prg.sendCellTemperatureRequest();
    //packetType=0;
  //}
}


void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(DIYBMSSoftAP::WifiSSID(), DIYBMSSoftAP::WifiPassword());
}

void connectToMqtt() {
  Serial1.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial1.println("Connected to Wi-Fi.");
  Serial1.print( WiFi.status() );
  Serial1.print(F(". Connected IP:"));
  Serial1.println(WiFi.localIP());

  /*
  TODO: CHECK ERROR CODES BETTER!
  0 : WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
  1 : WL_NO_SSID_AVAIL in case configured SSID cannot be reached
  3 : WL_CONNECTED after successful connection is established
  4 : WL_CONNECT_FAILED if password is incorrect
  6 : WL_DISCONNECTED if module is not configured in station mode
  */
    if (!server_running)
    {
      DIYBMSServer::StartServer(&server);
      server_running=true;
    }


  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial1.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  myTimerSendMqttPacket.detach();
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial1.println("Disconnected from MQTT.");

  myTimerSendMqttPacket.detach();

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

#define MQTT_HOST IPAddress(192, 168, 0, 26)
#define MQTT_PORT 1883

void sendMqttPacket() {
  Serial1.println("Sending MQTT");
  //publish(const char* topic, uint8_t qos, bool retain, const char* payload, size_t length, bool dup, uint16_t message_id) {

  char buffer[50];
  char value[50];

  for (uint8_t bank = 0; bank < 4; bank++) {
    for (uint16_t i = 0; i < numberOfModules[bank]; i++) {

      sprintf(buffer, "diybms/%d/%d/voltage", bank,i);
      float v=(float)cmi[bank][i].voltagemV/1000.0;
      dtostrf(v,7, 3, value);
      mqttClient.publish(buffer, 0, true, value);

      sprintf(buffer, "diybms/%d/%d/inttemp", bank,i);
      sprintf(value, "%d", cmi[bank][i].internalTemp);
      mqttClient.publish(buffer, 0, true, value);

      sprintf(buffer, "diybms/%d/%d/exttemp", bank,i);
      sprintf(value, "%d", cmi[bank][i].externalTemp);
      mqttClient.publish(buffer, 0, true, value);

      sprintf(buffer, "diybms/%d/%d/bypass", bank,i);
      sprintf(value, "%d", cmi[bank][i].inBypass ? 1:0);
      mqttClient.publish(buffer, 0, true, value);

      //cell["v"] = cmi[bank][i].voltagemV;
      //cell["bypass"] = cmi[bank][i].inBypass;
      //cell["bypasshot"] = cmi[bank][i].bypassOverTemp;
      //cell["int"] = cmi[bank][i].internalTemp;
      //cell["ext"] = cmi[bank][i].externalTemp;
    }
  }


}

void onMqttConnect(bool sessionPresent) {
  Serial1.println("Connected to MQTT.");
  myTimerSendMqttPacket.attach(15, sendMqttPacket);
}


void setup() {
  //Serial is used for communication to modules, Serial1 is for debug output

  pinMode(GREEN_LED, OUTPUT);
  GREEN_LED_OFF;

  numberOfModules[0]=0;
  numberOfModules[1]=0;
  numberOfModules[2]=0;
  numberOfModules[3]=0;

  //Pre configure the array
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

  //Ensure we service the cell modules every 5 seconds
  myTimer.attach(5, timerEnqueueCallback);

  //We process the transmit queue every 0.5 seconds
  myTransmitTimer.attach(0.5, timerTransmitCallback);

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

      wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
      wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

      mqttClient.onConnect(onMqttConnect);
      mqttClient.onDisconnect(onMqttDisconnect);
      //mqttClient.onSubscribe(onMqttSubscribe);
      //mqttClient.onUnsubscribe(onMqttUnsubscribe);
      //mqttClient.onMessage(onMqttMessage);
      //mqttClient.onPublish(onMqttPublish);
      mqttClient.setServer(MQTT_HOST, MQTT_PORT);
      mqttClient.setCredentials("emonpi","emonpimqtt2016");

      connectToWifi();
  }
}


void loop() {
  // Call update to receive, decode and process incoming packets.
  if (Serial.available()) {
    myPacketSerial.update();
  }

}

#ifndef DIYBMSServer_H_
#define DIYBMSServer_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <EEPROM.h>

#include "ArduinoJson.h"

class DIYBMSServer {
   public:
      static void StartServer(AsyncWebServer  *webserver);

  private:
      static AsyncWebServer * _myserver;

      static void handleRedirect();
      static void handleNotFound(AsyncWebServerRequest *request);
      static void monitor(AsyncWebServerRequest *request);
      static void sendHeaders();

};


extern bool waitingForReply;
extern uint8_t missedPacketCount;


#endif

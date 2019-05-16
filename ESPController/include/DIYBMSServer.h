
#ifndef DIYBMSServer_H_
#define DIYBMSServer_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <EEPROM.h>

#include "settings.h"
#include "ArduinoJson.h"
#include "PacketRequestGenerator.h"
#include "PacketReceiveProcessor.h"
#include "ESP8266TrueRandom.h"

class DIYBMSServer {
   public:
      static void StartServer(AsyncWebServer  *webserver);
      static void generateUUID();

  private:
      static AsyncWebServer * _myserver;
      static String UUIDString;

      static void handleNotFound(AsyncWebServerRequest *request);
      static void monitor(AsyncWebServerRequest *request);
      static void settings(AsyncWebServerRequest *request);
      static void integration(AsyncWebServerRequest *request);
      static void identifyModule(AsyncWebServerRequest *request);
      static void saveSetting(AsyncWebServerRequest *request);
      static void saveGlobalSetting(AsyncWebServerRequest *request);
      static String TemplateProcessor(const String& var);
      static void saveMQTTSetting(AsyncWebServerRequest *request);
      static void saveInfluxDBSetting(AsyncWebServerRequest *request);
      static bool validateXSS(AsyncWebServerRequest *request);
      static void SendSuccess(AsyncWebServerRequest *request);
};

extern PacketRequestGenerator prg;
extern PacketReceiveProcessor receiveProc;
extern diybms_eeprom_settings mysettings;

#endif

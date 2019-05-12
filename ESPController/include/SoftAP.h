#ifndef SOFTAP_H_
#define SOFTAP_H_




#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <EEPROM.h>
#include "settings.h"

//Where in EEPROM do we store the configuration
#define EEPROM_WIFI_START_ADDRESS 0

struct wifi_eeprom_settings {
  char wifi_ssid[32 + 1];
  char wifi_passphrase[63 + 1];
};

class DIYBMSSoftAP {
   public:
      static void SetupAccessPoint(AsyncWebServer  *webserver);
      static bool LoadConfigFromEEPROM();
      static char* WifiSSID();
      static char* WifiPassword();

  private:
      static AsyncWebServer * _myserver;
      static void handleNotFound(AsyncWebServerRequest *request);

      static void handleRoot(AsyncWebServerRequest *request);
      static void handleSave(AsyncWebServerRequest *request);


      static String htmlHeader();
      static String htmlFooter();

      static wifi_eeprom_settings _config;
      static String networks;
};


#endif

#include "SoftAP.h"


wifi_eeprom_settings DIYBMSSoftAP::_config;

String DIYBMSSoftAP::networks;

AsyncWebServer * DIYBMSSoftAP::_myserver;

char* DIYBMSSoftAP::WifiSSID() {
  return _config.wifi_ssid;
}
char* DIYBMSSoftAP::WifiPassword() {
  return _config.wifi_passphrase;
}

void DIYBMSSoftAP::handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


String DIYBMSSoftAP::htmlHeader() {
  return String(F("<!DOCTYPE HTML>\r\n<html><head><style>.page {width:300px;margin:0 auto 0 auto;background-color:cornsilk;font-family:sans-serif;padding:22px;} label {min-width:120px;display:inline-block;padding: 22px 0 22px 0;}</style></head><body><div class=\"page\"><h1>DIY BMS</h1>"));
}

String DIYBMSSoftAP::htmlFooter() {
  return String(F("</div></body></html>\r\n\r\n"));
}

void DIYBMSSoftAP::handleRoot(AsyncWebServerRequest *request)
{
  String s;
  s = htmlHeader();
  //F Macro - http://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
  s += F("<h2>WiFi Setup</h2><p>Select local WIFI to connect to:</p><form autocomplete=\"off\" method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"\\save\"><label for=\"ssid\">SSID:</label><select id=\"ssid\" name=\"ssid\">");
  s += DIYBMSSoftAP::networks;
  s += F("</select><label for=\"pass\">Password:</label><input type=\"password\" id=\"id\" name=\"pass\"><br/><input minlength=\"8\" maxlength=\"32\" type=\"submit\" value=\"Submit\"></form>");
  s += htmlFooter();
  request->send(200, "text/html", s);
}

void DIYBMSSoftAP::handleSave(AsyncWebServerRequest *request) {
  String s;
  String ssid = request->arg("ssid");
  String password = request->arg("pass");

  if ((ssid.length() <= sizeof(_config.wifi_ssid)) && (password.length() <= sizeof(_config.wifi_passphrase))) {

    memset(&_config, 0, sizeof(_config));

    ssid.toCharArray(_config.wifi_ssid, sizeof(_config.wifi_ssid));
    password.toCharArray(_config.wifi_passphrase, sizeof(_config.wifi_passphrase));

    Settings::WriteConfigToEEPROM((char*)&_config, sizeof(_config), EEPROM_WIFI_START_ADDRESS);

    s = htmlHeader() + F("<p>WIFI settings saved, will reboot in 5 seconds.</p>") + htmlFooter();

    request->send(200, "text/html", s);


    //Serial1.println("handleSave");
    //Serial1.println(_config.wifi_ssid);
    //Serial1.println(_config.wifi_passphrase);
    
    //Delay 6 seconds
    for (size_t i = 0; i < 60; i++) {    delay(100);  }

    ESP.restart();

  } else {
    s = htmlHeader() + F("<p>WIFI settings too long.</p>") + htmlFooter();

    request->send(200, "text/html", s);
  }
}

bool DIYBMSSoftAP::LoadConfigFromEEPROM() {
   return (Settings::ReadConfigFromEEPROM((char*)&_config, sizeof(_config), EEPROM_WIFI_START_ADDRESS));
}

void DIYBMSSoftAP::SetupAccessPoint(AsyncWebServer  *webserver) {

  _myserver=webserver;
  const char* ssid = "DIY_BMS_CONTROLLER";

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);
  int n = WiFi.scanNetworks();

  if (n == 0)
    DIYBMSSoftAP::networks = "no networks found";
  else
  {
    for (int i = 0; i < n; ++i)
    {
      if (WiFi.encryptionType(i) != ENC_TYPE_NONE) {
        // Only show encrypted networks
        DIYBMSSoftAP::networks += "<option>"+WiFi.SSID(i)+"</option>";
      }
      delay(10);
    }
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  _myserver->on("/", HTTP_GET, handleRoot);
  _myserver->on("/save", HTTP_POST, handleSave);
  _myserver->onNotFound(handleNotFound);
  _myserver->begin();
}

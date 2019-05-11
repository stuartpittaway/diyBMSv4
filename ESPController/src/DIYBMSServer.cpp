/*
  ____  ____  _  _  ____  __  __  ___
 (  _ \(_  _)( \/ )(  _ \(  \/  )/ __)
  )(_) )_)(_  \  /  ) _ < )    ( \__ \
 (____/(____) (__) (____/(_/\/\_)(___/

DIYBMS V4.0
ESP8266 MODULE

(c)2019 Stuart Pittaway

COMPILE THIS CODE USING PLATFORM.IO

LICENSE
Attribution-NonCommercial-ShareAlike 2.0 UK: England & Wales (CC BY-NC-SA 2.0 UK)
https://creativecommons.org/licenses/by-nc-sa/2.0/uk/

* Non-Commercial — You may not use the material for commercial purposes.
* Attribution — You must give appropriate credit, provide a link to the license,
and indicate if changes were made.
  You may do so in any reasonable manner, but not in any way that suggests the
licensor endorses you or your use.
* ShareAlike — If you remix, transform, or build upon the material, you must
distribute your   contributions under the same license as the original.
* No additional restrictions — You may not apply legal terms or technological measures
  that legally restrict others from doing anything the license permits.

*/
#include "DIYBMSServer.h"
#include "ArduinoJson.h"
#include "defines.h"
#include "ESP8266TrueRandom.h"

#include "html_1.h"
#include "jquery.h"
#include "logo.h"
#include "echarts_js.h"

AsyncWebServer *DIYBMSServer::_myserver;
String DIYBMSServer::UUIDString;


void DIYBMSServer::generateUUID() {
    Serial1.print("generateUUID=");
    byte uuidNumber[16]; // UUIDs in binary form are 16 bytes long
    ESP8266TrueRandom.uuid(uuidNumber);
    UUIDString = ESP8266TrueRandom.uuidToString(uuidNumber);

    Serial1.println(UUIDString);
}

void DIYBMSServer::saveGlobalSetting(AsyncWebServerRequest *request) {
  if (request->hasParam("BypassOverTempShutdown", true) && request->hasParam("BypassThresholdmV", true)) {

    AsyncWebParameter *p1 = request->getParam("BypassOverTempShutdown", true);
    uint8_t BypassOverTempShutdown=p1->value().toInt();

    AsyncWebParameter *p2 = request->getParam("BypassThresholdmV", true);
    uint16_t BypassThresholdmV=p2->value().toInt();

    prg.sendSaveGlobalSetting(BypassThresholdmV,BypassOverTempShutdown);

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    DynamicJsonDocument doc(2048);

    serializeJson(doc, *response);
    request->send(response);

  } else {
    request->send(500, "text/plain", "Missing parameters");
  }
}


void DIYBMSServer::handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void DIYBMSServer::saveSetting(AsyncWebServerRequest *request) {
  if (request->hasParam("m", true) && request->hasParam("b", true)) {

    AsyncWebParameter *module = request->getParam("m", true);
    AsyncWebParameter *bank = request->getParam("b", true);

    int b = bank->value().toInt();
    int m = module->value().toInt();

    if ((b > 3) || (m + 1 > numberOfModules[b])) {
      request->send(500, "text/plain", "Wrong parameters");
    } else {

    uint8_t BypassOverTempShutdown=0xFF;
    uint16_t BypassThresholdmV=0xFFFF;

    // Resistance of bypass load
    float LoadResistance=0xFFFF;
    //Voltage Calibration
    float Calibration=0xFFFF;
    //Reference voltage (millivolt) normally 2.00mV
    float mVPerADC=0xFFFF;
    //Internal Thermistor settings
    uint16_t Internal_BCoefficient=0xFFFF;
    //External Thermistor settings
    uint16_t External_BCoefficient=0xFFFF;

    if (request->hasParam("BypassOverTempShutdown", true)) {
      AsyncWebParameter *p1 = request->getParam("BypassOverTempShutdown", true);
      BypassOverTempShutdown=p1->value().toInt();
    }

    if (request->hasParam("BypassThresholdmV", true)) {
      AsyncWebParameter *p1 = request->getParam("BypassThresholdmV", true);
      BypassThresholdmV=p1->value().toInt();
    }
    if (request->hasParam("IntBCoef", true)) {
      AsyncWebParameter *p1 = request->getParam("IntBCoef", true);
      Internal_BCoefficient=p1->value().toInt();
    }

    if (request->hasParam("ExtBCoef", true)) {
      AsyncWebParameter *p1 = request->getParam("ExtBCoef", true);
      External_BCoefficient=p1->value().toInt();
    }

    if (request->hasParam("Calib", true)) {
      AsyncWebParameter *p1 = request->getParam("Calib", true);
      Calibration=p1->value().toFloat();
    }

    if (request->hasParam("LoadRes", true)) {
      AsyncWebParameter *p1 = request->getParam("LoadRes", true);
      LoadResistance=p1->value().toFloat();
    }

    if (request->hasParam("mVPerADC", true)) {
      AsyncWebParameter *p1 = request->getParam("mVPerADC", true);
      mVPerADC=p1->value().toFloat();
    }

    prg.sendSaveSetting(b, m,BypassThresholdmV,BypassOverTempShutdown,LoadResistance,Calibration,mVPerADC,Internal_BCoefficient,External_BCoefficient);

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    DynamicJsonDocument doc(2048);
    serializeJson(doc, *response);
    request->send(response);
}
  } else {
    request->send(500, "text/plain", "Missing parameters");
  }
}

void DIYBMSServer::identifyModule(AsyncWebServerRequest *request) {
  if (request->hasParam("m") && request->hasParam("b")) {
    AsyncWebParameter *module = request->getParam("m");
    AsyncWebParameter *bank = request->getParam("b");

    int b = bank->value().toInt();
    int m = module->value().toInt();

    if ((b > 3) || (m + 1 > numberOfModules[b])) {
      request->send(500, "text/plain", "Wrong parameters");
    } else {
      prg.sendIdentifyModuleRequest(b, m);

      
      AsyncResponseStream *response =
          request->beginResponseStream("application/json");

      DynamicJsonDocument doc(2048);
      JsonObject root = doc.to<JsonObject>();
      JsonObject a = root.createNestedObject("identifyModule");

      a["bank"] = b;
      a["module"] = m;

      serializeJson(doc, *response);
      request->send(response);
    }

  } else {
    request->send(500, "text/plain", "Missing parameters");
  }
}
void DIYBMSServer::settings(AsyncWebServerRequest *request) {
  if (request->hasParam("m", false) && request->hasParam("b", false)) {
    AsyncWebParameter *module = request->getParam("m", false);
    AsyncWebParameter *bank = request->getParam("b", false);

    int b = bank->value().toInt();
    int m = module->value().toInt();

    if ((b > 3) || (m + 1 > numberOfModules[b])) {
      request->send(500, "text/plain", "Wrong parameters");
    } else {
      if (cmi[b][m].settingsCached == false) {
        prg.sendGetSettingsRequest(b, m);
      }

      AsyncResponseStream *response =
          request->beginResponseStream("application/json");

      DynamicJsonDocument doc(2048);
      JsonObject root = doc.to<JsonObject>();
      JsonObject settings = root.createNestedObject("settings");

      settings["bank"] = b;
      settings["module"] = m;

      settings["Cached"] = cmi[b][m].settingsCached;
      // settings["Requested"] = cmi[b][m].settingsRequested;

      settings["BypassOverTempShutdown"] = cmi[b][m].BypassOverTempShutdown;
      settings["BypassThresholdmV"] = cmi[b][m].BypassThresholdmV;

      settings["LoadRes"] = cmi[b][m].LoadResistance;
      settings["Calib"] = cmi[b][m].Calibration;
      settings["mVPerADC"] = cmi[b][m].mVPerADC;
      settings["IntBCoef"] = cmi[b][m].Internal_BCoefficient;
      settings["ExtBCoef"] = cmi[b][m].External_BCoefficient;

      serializeJson(doc, *response);
      request->send(response);
    }
  } else {
    request->send(500, "text/plain", "Missing parameters");
  }
}

void DIYBMSServer::monitor(AsyncWebServerRequest *request) {
  AsyncResponseStream *response =
      request->beginResponseStream("application/json");

  DynamicJsonDocument doc(4096);

  JsonObject root = doc.to<JsonObject>();

  JsonObject monitor = root.createNestedObject("monitor");

  // Set error flag if we have attempted to send 2 without a reply
  monitor["commserr"] = (receiveProc.commsError > 2);

  monitor["badpkt"] = receiveProc.totalMissedPacketCount;
  monitor["badcrc"] = receiveProc.totalCRCErrors;
  monitor["ignored"] = receiveProc.totalNotProcessedErrors;

  JsonArray bankArray = root.createNestedArray("bank");

  for (uint8_t bank = 0; bank < 4; bank++) {
    JsonArray data = bankArray.createNestedArray();
    for (uint16_t i = 0; i < numberOfModules[bank]; i++) {
      JsonObject cell = data.createNestedObject();
      cell["v"] = cmi[bank][i].voltagemV;
      cell["minv"] = cmi[bank][i].voltagemVMin;
      cell["maxv"] = cmi[bank][i].voltagemVMax;
      cell["bypass"] = cmi[bank][i].inBypass;
      cell["bypasshot"] = cmi[bank][i].bypassOverTemp;
      cell["int"] = cmi[bank][i].internalTemp;
      cell["ext"] = cmi[bank][i].externalTemp;
    }
  }
  serializeJson(doc, *response);
  request->send(response);
}

String DIYBMSServer::TemplateProcessor(const String& var)
{
  if(var == "XSS_KEY")
    return DIYBMSServer::UUIDString;

  return String();
}

void DIYBMSServer::StartServer(AsyncWebServer *webserver) {

  _myserver = webserver;

  String cookieValue="DIYBMS_XSS=";
  cookieValue+=DIYBMSServer::UUIDString;
  DefaultHeaders::Instance().addHeader("Set-Cookie", cookieValue);

  _myserver->on("/monitor.json", HTTP_GET, DIYBMSServer::monitor);

  _myserver->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", FILE_INDEX_HTML, DIYBMSServer::TemplateProcessor);
    request->send(response);
  });

  // Return GZIP'ed JQUERY code to browser
  _myserver->on("/jquery.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(
        200, "text/javascript", FILE_JQUERY, FILE_JQUERY_SIZE_BYTES);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  _myserver->on("/logo.gif", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(
        200, "image/gif", FILE_LOGO, FILE_LOGO_SIZE_BYTES);
    request->send(response);
  });

  _myserver->on(
      "/echarts.simple.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(
            200, "text/javascript", FILE_ECHARTS, FILE_ECHARTS_SIZE_BYTES);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
      });

  _myserver->on("/settings.json", HTTP_GET, DIYBMSServer::settings);
  _myserver->on("/identifyModule.json", HTTP_GET, DIYBMSServer::identifyModule);
  _myserver->on("/savesetting.json", HTTP_POST, DIYBMSServer::saveSetting);
  _myserver->on("/saveglobalsetting.json", HTTP_POST, DIYBMSServer::saveGlobalSetting);

  _myserver->onNotFound(DIYBMSServer::handleNotFound);
  _myserver->begin();
}

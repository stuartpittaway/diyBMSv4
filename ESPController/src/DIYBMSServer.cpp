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
* Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
  You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
* ShareAlike — If you remix, transform, or build upon the material, you must distribute your
  contributions under the same license as the original.
* No additional restrictions — You may not apply legal terms or technological measures
  that legally restrict others from doing anything the license permits.

*/
#include "DIYBMSServer.h"

#include "ArduinoJson.h"

#include "defines.h"

AsyncWebServer * DIYBMSServer::_myserver;

#include "html_1.h"
#include "jquery.h"
#include "logo.h"
#include "echarts_js.h"

void DIYBMSServer::handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void DIYBMSServer::identifyModule(AsyncWebServerRequest *request) {
  if(request->hasParam("m", false) && request->hasParam("b", false)){

    AsyncWebParameter* module = request->getParam("m", false);
    AsyncWebParameter* bank = request->getParam("b", false);

    int b=bank->value().toInt();
    int m=module->value().toInt();

    if ((b>3) || (m+1>numberOfModules[b]))
    {
      request->send(500, "text/plain", "Wrong parameters");
    } else {

      prg.sendIdentifyModuleRequest(b,m);

      /*
      if (cmi[b][m].identifyModule==false) {
        cmi[b][m].identifyModule=true;
        requestPending=true;
      }
      */

      AsyncResponseStream *response = request->beginResponseStream("application/json");

      DynamicJsonDocument doc(2048);
      JsonObject root = doc.to<JsonObject>();
      JsonObject a = root.createNestedObject("identifyModule");

      a["bank"]=b;
      a["module"]=m;

      serializeJson(doc, *response);
      request->send(response);
}

} else {

    request->send(500, "text/plain", "Missing parameters");
}
}
void DIYBMSServer::settings(AsyncWebServerRequest *request) {

  if(request->hasParam("m", false) && request->hasParam("b", false)){

  AsyncWebParameter* module = request->getParam("m", false);
  AsyncWebParameter* bank = request->getParam("b", false);

  int b=bank->value().toInt();
  int m=module->value().toInt();

  if ((b>3) || (m+1>numberOfModules[b]))
  {
    request->send(500, "text/plain", "Wrong parameters");
  } else {

    if (cmi[b][m].settingsCached==false) {
      prg.sendGetSettingsRequest(b,m);
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    DynamicJsonDocument doc(2048);
    JsonObject root = doc.to<JsonObject>();
    JsonObject settings = root.createNestedObject("settings");

    settings["bank"]=b;
    settings["module"]=m;

    settings["Cached"] = cmi[b][m].settingsCached;
    settings["Requested"] = cmi[b][m].settingsRequested;

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
  AsyncResponseStream *response = request->beginResponseStream("application/json");

  DynamicJsonDocument doc(4096);

  JsonObject root = doc.to<JsonObject>();

  JsonObject monitor = root.createNestedObject("monitor");

  //Set error flag if we have sent 5 packets without a reply
  monitor["commserr"]=(commsError>5);

  monitor["badpkt"]=receiveProc.totalMissedPacketCount;
  monitor["badcrc"]=receiveProc.totalCRCErrors;
  monitor["ignored"]=receiveProc.totalNotProcessedErrors;

JsonArray bankArray = root.createNestedArray("bank");

  for (uint8_t bank = 0; bank < 4; bank++) {

    JsonArray data = bankArray.createNestedArray();
  for (uint16_t i = 0; i < numberOfModules[bank]; i++) {
    JsonObject cell=data.createNestedObject();
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


void DIYBMSServer::StartServer(AsyncWebServer *webserver) {
  _myserver=webserver;

  _myserver->on("/monitor.json", HTTP_GET, DIYBMSServer::monitor);

  _myserver->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", FILE_INDEX_HTML);
  });

  //Return GZIP'ed JQUERY code to browser
  _myserver->on("/jquery.js", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200,  "text/javascript", FILE_JQUERY, FILE_JQUERY_SIZE_BYTES);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  _myserver->on("/logo.gif", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200,  "image/gif", FILE_LOGO,FILE_LOGO_SIZE_BYTES);
    request->send(response);
  });

  _myserver->on("/echarts.simple.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200,  "text/javascript", FILE_ECHARTS, FILE_ECHARTS_SIZE_BYTES);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });


  _myserver->on("/settings.json", HTTP_GET, DIYBMSServer::settings);
  _myserver->on("/identifyModule.json", HTTP_GET, DIYBMSServer::identifyModule);


  _myserver->onNotFound(DIYBMSServer::handleNotFound);
  _myserver->begin();
}

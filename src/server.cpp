
#include "server.h"
#include "dimmer.h"
#include <LittleFS.h>

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// manual dimmer control via web
void dimmerRequest(AsyncWebServerRequest *request) {
  String power;
  if (request->hasParam(POWER_PARAM)) {
      power = request->getParam(POWER_PARAM)->value();
      setDimmer(power);
  } else {
      power = "No message sent";
  }
  request->send(200, "text/json", "status: {led: 'Turning Built In LED " + power + "'}");
}

AsyncWebServer* initServer() {

  // Webserver
  AsyncWebServer* server = new AsyncWebServer(80);

  // Begin LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return NULL;
  }

  // Route for root index.html
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  // Route for root index.css
  server->on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.css", "text/css"); });

  // Route for root entireframework.min.css
  server->on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/entireframework.min.css", "text/css"); });

  // Route for root index.js
  server->on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", "text/javascript"); });

  // favicon.ico
  server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/favicon.ico", "image/png");
  });

  // Respond to dimmer event
  server->on("/dimmer", HTTP_GET, dimmerRequest);

  server->onNotFound(notFound);

  return server;
}


#include "server.h"
#include "wifi.h"
#include "mqtt.h"
#include "dimmer.h"
#include <AsyncElegantOTA.h>

const String WIFI_SSID = "freeboxhd";
const String WIFI_PASSWORD = "01002003004005F";
Ticker serverTimer;
AsyncWebServer* server = NULL;

void setup() {

  Serial.begin(921600);

    Serial.println("------------>" + String(ESP.getSdkVersion()));

  // Webserver
  server = initServer();

  if (server != NULL) {
    // Connect to WIFI
    setup_wifi(WIFI_SSID, WIFI_PASSWORD);

    // setup mqtt
    setup_mqtt();

    // dimmer
    setup_dimmer();

    // ElegantOTA (accessible via /update)
    AsyncElegantOTA.begin(server);

    serverTimer.attach(600, []() {
      Serial.println("------------->- Restaring server..." + String(ESP.getFreeHeap()));
      delete server;
      server = initServer();
      AsyncElegantOTA.begin(server);
      server->begin();
    });

    // start the server
    server->begin();
  }
}

void loop() {
}


#include "server.h"
#include "wifi.h"
#include "mqtt.h"
#include "dimmer.h"
#include <AsyncElegantOTA.h>
#include <WebSerial.h>

const String WIFI_SSID = "freeboxhd";
const String WIFI_PASSWORD = "01002003004005F";
Ticker serverTimer;
AsyncWebServer* server = NULL;

void setup2() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);
}

void reloadWebServer() {
  delete server;
  server = initServer();
  WebSerial.begin(server);
  AsyncElegantOTA.begin(server);
  server->begin();
}

void setup() {

  Serial.begin(921600);

  // Webserver
  server = initServer();

  if (server != NULL) {
    // Connect to WIFI
    setup_wifi(WIFI_SSID, WIFI_PASSWORD);

    // setup mqtt
    setup_mqtt();

    // dimmer
    setup_dimmer();

    // WebSerial
    WebSerial.begin(server);

    // ElegantOTA (accessible via /update)
    AsyncElegantOTA.begin(server);

    serverTimer.attach(60, []() {
      Serial.println("------------->- Restaring server..." + String(ESP.getFreeHeap()));
      // reloadWebServer();
    });

    // start the server
    server->begin();
  }
}

void loop() {
}

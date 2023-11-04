
#include "server.h"
#include "wifi.h"
#include "mqtt.h"
#include "dimmer.h"
#include <AsyncElegantOTA.h>
#include "webserial.h"

const String WIFI_SSID = "freeboxhd";
const String WIFI_PASSWORD = "01002003004005F";
const char* MQTT_LOGIN = "dreambo";
const char* MQTT_PASSWORD = "bouBouS01";
Ticker serverTimer;
AsyncWebServer* server = NULL;


void reloadWebServer() {
  print(" ----> restarting web server ....");
  /*
  delete server;
  server = initServer();
  setup_webserial(server);
  AsyncElegantOTA.begin(server);
  server->begin();
  */
}

void setup() {

  Serial.begin(921600);

  // Webserver
  server = initServer();

  if (server != NULL) {
    // Connect to WIFI
    setup_wifi(WIFI_SSID, WIFI_PASSWORD);
  
    while (!WiFi.isConnected()) {
      Serial.print(".");
      delay(3000);
    }

    // WebSerial
    setup_webserial(server);

    // setup mqtt
    setup_mqtt(MQTT_LOGIN, MQTT_PASSWORD);

    // dimmer
    setup_dimmer();

    // ElegantOTA (accessible via /update)
    AsyncElegantOTA.begin(server);

    serverTimer.attach(600, reloadWebServer);

    // start the server
    server->begin();
  }
}

void loop() {
}

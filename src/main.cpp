
/*
  Title:  ESP8266 Webserver - No Filesystem
  Description:  An ESP8266 webserver that uses LittleFS to load web content
  Author: donsky
  For:    www.donskytech.com
  Date:   September 14, 2022
*/
#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

const char *ssid = "freeboxhd";
const char *password = "01002003004005F";
const char* PARAM_MESSAGE = "status";
volatile uint8_t last = 0, lastVal = 0, dimPower = 0, count = 0;

// Webserver
AsyncWebServer server(80);

const int TRIAC_PIN = D1;
const int ZCV_PIN = D2;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void toggleLED(String status) {
    Serial.println("Status: " + status);
    dimPower = map(status.toInt(), 0, 11, 0, 90);
}

void IRAM_ATTR ISR() {
  int val = ((uint16_t)++count * dimPower) >> 8;		
  if (lastVal != (val != last)) {
    digitalWrite(TRIAC_PIN, val != last);
  }
  lastVal = (val != last);
  last = val;
}

void setup() {

  Serial.begin(921600);
  pinMode(TRIAC_PIN, OUTPUT);
  pinMode(ZCV_PIN, INPUT_PULLUP);

  // Begin LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!, retrying...");
    delay(000);
    WiFi.begin(ssid, password);
  }

  Serial.println("Starting the LittleFS Webserver..");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  digitalWrite(TRIAC_PIN, 0);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  // Route for root index.css
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.css", "text/css"); });

  // Route for root entireframework.min.css
  server.on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/entireframework.min.css", "text/css"); });

  // Route for root index.js
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", "text/javascript"); });

  // Respond to toggle event
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    String status;
    if (request->hasParam(PARAM_MESSAGE)) {
        status = request->getParam(PARAM_MESSAGE)->value();
        toggleLED(status);
    } else {
        status = "No message sent";
    }
    request->send(200, "text/json", "{led: 'Turning Built In LED " + status + "'}");
  });

  server.onNotFound(notFound);

  // dimmer
  attachInterrupt(ZCV_PIN, ISR, RISING);  // D2 == 0

  // ElegantOTA
  AsyncElegantOTA.begin(&server);

  // start the server
  server.begin();
}

void loop() {
}

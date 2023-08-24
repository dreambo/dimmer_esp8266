/*
  WebSerial Demo AP
  ------
  This example code works for both ESP8266 & ESP32 Microcontrollers
  WebSerial is accessible at 192.168.4.1/webserial URL.

  Author: Ayush Sharma
  Checkout WebSerial Pro: https://webserial.pro
*/
#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer server1(80);

const char* ssid1 = "freeboxhd"; // Your WiFi AP SSID 
const char* password1 = "01002003004005F"; // Your WiFi Password


/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

void setup_wifi1() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  delay(10);
  Serial.println("Connecting to " + String(ssid1));

  WiFi.begin(ssid1, password1);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN, 1);

  Serial.println("WiFi connected: " + WiFi.localIP().toString());
}

void setup_webserial() {
    Serial.begin(921600);
    setup_wifi1();
    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&server1);
    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);
    server1.begin();
}

void loop_webserial() {
    delay(2000);
    
    WebSerial.print(F("IP address: "));
    WebSerial.println(WiFi.localIP());
    WebSerial.printf("Millis=%lu\n", millis());
    WebSerial.printf("Free heap=[%u]\n", ESP.getFreeHeap());
}

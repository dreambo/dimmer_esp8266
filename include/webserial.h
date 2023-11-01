#ifndef WEB_SERIAL__H
  #include <Arduino.h>
  #if defined(ESP8266)
    #include <ESPAsyncTCP.h>
  #elif defined(ESP32)
    #include <AsyncTCP.h>
  #endif
  #include <ESPAsyncWebServer.h>
  #include <WebSerial.h>

  void setup_webserial(AsyncWebServer* server);
  void print(String txt);
#endif
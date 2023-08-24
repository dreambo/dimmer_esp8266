#ifndef WI_FI__H
    #define WI_FI__H
    #include <Arduino.h>
    #include <Ticker.h>
    #include "server.h"
    #if defined(ESP8266)
        #include <ESP8266WiFi.h>
    #elif defined(ESP32)
        #include <WiFi.h>
    #endif

    void setup_wifi(String ssId, String passWord);
    String scanWifi();
#endif

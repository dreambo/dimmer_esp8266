#ifndef DIMMER_H
    #define DIMMER_H
    #include <Arduino.h>
    #if defined(ESP8266)
        #define HARDWARE "ESP8266"
    #elif defined(ESP32)
        #define HARDWARE "ESP32"
    #endif

    void setup_dimmer();
    void setDimmer(String power);
#endif
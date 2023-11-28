#ifndef SERVER_H
    #define SERVER_H
    #include "ESPAsyncWebServer.h"
    #define POWER_PARAM "power"
    #if defined(ESP8266)
        #include "ESP8266WiFi.h"
        #include "ESPAsyncTCP.h"
    #elif defined(ESP32)
        #include "WiFi.h"
        #include "AsyncTCP.h"
    #endif

    AsyncWebServer* initServer();
    boolean restartServer();
#endif

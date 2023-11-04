#ifndef MQTT_H
    #define MQTT_H
    #include <Arduino.h>
    #include <Ticker.h>
    #include <AsyncMqttClient.h>

    void setup_mqtt(const char* login, const char* password);
#endif

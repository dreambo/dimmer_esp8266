#include "wifi.h"

String ssid, password;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi " + ssid + " ...");
  digitalWrite(LED_BUILTIN, ON);
  WiFi.begin(ssid, password);
}

String scanWifi() {
  String json = "[";
  int n = WiFi.scanComplete();
  if (n == -2) {
    WiFi.scanNetworks(true);
  } else if(n) {
    for (int i = 0; i < n; ++i) {
      if (i) json += ",";
      json += "{";
      json += "rssi:" + String(WiFi.RSSI(i));
      json += ",ssid:\"" + WiFi.SSID(i) + "\"";
      json += ",bssid:\"" + WiFi.BSSIDstr(i) + "\"";
      json += ",channel:" + String(WiFi.channel(i));
      json += ",secure:" + String(WiFi.encryptionType(i));
      json += "}";
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }

  return (json += "]");
}

#if defined(ESP8266)

  WiFiEventHandler wifiConnectHandler;
  WiFiEventHandler wifiDisconnectHandler;

  void onWifiConnect(const WiFiEventStationModeGotIP& event) {
    Serial.println("Connected to Wi-Fi: " + WiFi.localIP().toString());
    digitalWrite(LED_BUILTIN, OFF);
  }

  void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    Serial.println("Disconnected from Wi-Fi, retrying ...");
    connectToWifi();
  }

  void setup_wifi(String ssId, String passWord) {
    ssid = ssId;
    password = passWord;

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, OFF);

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    connectToWifi();
  }

#elif defined(ESP32)

  void wifiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    delay(500);
    Serial.println("Connected.");
  }

  void wifiDisConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Disconnected from Wi-Fi, retrying ...");
    connectToWifi();
  }

  void wifiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    delay(500);
    Serial.println("Local IP: " + WiFi.localIP().toString());
    digitalWrite(LED_BUILTIN, OFF);
  }

  void setup_wifi(String ssId, String passWord) {
    ssid = ssId;
    password = passWord;

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ON);

    WiFi.mode(WIFI_STA);
    WiFi.onEvent(wifiConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(wifiDisConnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(wifiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);

    connectToWifi();
  }

#endif



#include "mqtt.h"
#include "wifi.h"
#include "dimmer.h"
#include "webserial.h"

#define MQTT_HOST IPAddress(192,168,0,6)
#define MQTT_PORT 1883

// MQTT Topics
const char* MQTT_SUB_HOME_POWER = "smg-ii/sensor/smg-ii_output_active_power/state";
const char* MQTT_SUB_BATTERY_STATE = "smg-ii/sensor/smg-ii_battery_state_of_charge/state";
const char* MQTT_SUB_BATTERY_POWER = "smg-ii/sensor/smg-ii_battery_average_power/state";
const char* MQTT_SUB_DIM = "smg-ii/number/dimmer/state";
const char* MQTT_PUB_DIM = "smg-ii/light/dimmerized_light/state";
const char* MQTT_SUB_MAINS = "smg-ii/sensor/smg-ii_average_mains_power/state";

// Variables to hold sensor readings
int homePower = 973;
int batteryState = 85;
int batteryPower = 0;
int mainsPower = 0;
int dimmerValue;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker timer;

unsigned long currentMillis, previousMillis = 0;
const unsigned long interval = 4000;

void connectToMqtt() {
  print("Connecting to MQTT...");
  mqttClient.connect();
}

void mqttSubscribe() {
  mqttClient.subscribe(MQTT_SUB_BATTERY_POWER, 1); // power of battery (negatif if discharging)
  mqttClient.subscribe(MQTT_SUB_BATTERY_STATE, 1); // percent of battery charge 
  mqttClient.subscribe(MQTT_SUB_HOME_POWER, 1);    // inverter output active power 
  mqttClient.subscribe(MQTT_SUB_DIM, 1);           // manual set of percent of dimmer power
  mqttClient.subscribe(MQTT_SUB_MAINS, 1);         // mains power
}

void setDimmerValue() {

  String log = "batteryState: " + String(batteryState)+ " --> batteryPower: " + String(batteryPower) + 
               " --> homePower: " + String(homePower) + " --> mainsPower: " + String(mainsPower);
  print(log);

  if (mainsPower > 0) {
    dimmerValue = 0;
  } else if (batteryPower < 0) {
    dimmerValue -= 4;
    if (dimmerValue < 1) dimmerValue = 0;
  } else {
    if (batteryPower < 50) {
      if (batteryState < 80) {
        dimmerValue = 0;
      } else {
        dimmerValue += 4;
        if (dimmerValue > 99) dimmerValue = 100;
      }
    } else {
      dimmerValue = (batteryPower - 30)/25;
    }
  }

  // setDimmer(String(dimmerValue));
}

void onMqttConnect(bool sessionPresent) {
  print("Connected to MQTT.");
  print("Session present: ");
  print(String(sessionPresent));

  mqttSubscribe();
  timer.attach(10, setDimmerValue);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  print("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  print("Subscribe acknowledged.");
  print("  packetId: ");
  print(String(packetId));
  print("  qos: ");
  print(String(qos));
}

void onMqttUnsubscribe(uint16_t packetId) {
  print("Unsubscribe acknowledged.");
  print("  packetId: ");
  print(String(packetId));
}

void onMqttPublish(uint16_t packetId) {
  print("Publish acknowledged.");
  print("  packetId: ");
  print(String(packetId));
}

String toString(char *data, unsigned int length) {
  String d = "";
  for (unsigned int i = 0; i < length; i++) {
    d += data[i];
  }

  return d;
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  /*
  print("Publish received.");
  print("  topic: ");
  print(topic);
  print("  qos: ");
  print(properties.qos);
  print("  dup: ");
  print(properties.dup);
  print("  retain: ");
  print(properties.retain);
  print("  len: ");
  print(len);
  print("  index: ");
  print(index);
  print("  total: ");
  print(total);
  */

  // print(String(topic) + " --> " + toString(payload, len));

  if (strcmp(topic, MQTT_SUB_BATTERY_POWER) == 0) {
    batteryPower = toString(payload, len).toInt();
  } else if (strcmp(topic, MQTT_SUB_BATTERY_STATE) == 0) {
    batteryState = toString(payload, len).toInt();
  } else if (strcmp(topic, MQTT_SUB_HOME_POWER) == 0) {
    homePower = toString(payload, len).toInt();
  } else if (strcmp(topic, MQTT_SUB_MAINS) == 0) {
    mainsPower = toString(payload, len).toInt();
  } else if (strcmp(topic, MQTT_SUB_DIM) == 0) {
    dimmerValue = toString(payload, len).toInt();
    setDimmer(String(dimmerValue));
    // mqttClient.publish(MQTT_PUB_DIM, 1, true, String(dimmerValue).c_str());
    return;
  }
}

void setup_mqtt() {

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials("dreambo", "bouBouS01");

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  // mqttClient.onPublish(onMqttPublish);
  mqttClient.onMessage(onMqttMessage);

  connectToMqtt();
}

void mqttPublish() {
    uint16_t packetIdPub = mqttClient.publish(MQTT_SUB_HOME_POWER, 1, true, String(homePower).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_SUB_HOME_POWER, packetIdPub);
    Serial.printf("Message: %.2f \n", homePower);

    // Publish an MQTT message on topic esp/bme680/batteryState
    packetIdPub = mqttClient.publish(MQTT_SUB_BATTERY_STATE, 1, true, String(batteryState).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_SUB_BATTERY_STATE, packetIdPub);
    Serial.printf("Message: %.2f \n", batteryState);

    // Publish an MQTT message on topic esp/bme680/batteryPower
    packetIdPub = mqttClient.publish(MQTT_SUB_BATTERY_POWER, 1, true, String(batteryPower).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_SUB_BATTERY_POWER, packetIdPub);
    Serial.printf("Message: %.2f \n", batteryPower);

    // Publish an MQTT message on topic esp/bme680/gas
    packetIdPub = mqttClient.publish(MQTT_SUB_DIM, 1, true, String(dimmerValue).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_SUB_DIM, packetIdPub);
    Serial.printf("Message: %.2f \n", dimmerValue);
}


#include "dimmer.h"

volatile boolean zero = true;
volatile boolean cent = false;
volatile uint16_t MIN_MICROS = 50;
volatile uint16_t MAX_MICROS = 9500;
volatile uint8_t last = 0, lastVal = 0, count = 0;
volatile uint16_t dimmerPower = MAX_MICROS;
volatile unsigned long lastMillis = 0;
volatile unsigned long now;

const int TRIAC_PIN = (HARDWARE == "ESP8266" ? 5 : 12);
const int ZCV_PIN = (HARDWARE == "ESP8266" ? 4 : 14);
const int POT_PIN = A0;

// set the dimmer value, the dimmer output power
void setDimmer(String power) {
    zero = (power == "0");
    cent = (power == "100");
    dimmerPower = map(power.toInt(), 0, 100, MAX_MICROS, MIN_MICROS);
    Serial.println("Power: " + power + ", dimmerPower: " + String(dimmerPower) + " -- max micros: " + String(MAX_MICROS));
    // dimmerPower = map(status.toInt(), 0, 100, 0, 90); // for Bresenham dimmer
}

// phase dimmer
void IRAM_ATTR ISR_PHASE() {
  // MAX_MICROS = ((now = micros()) - lastMillis) - 3*MIN_MICROS;
  // lastMillis = now;

  if (cent) {
    digitalWrite(TRIAC_PIN, 1);
  } else if (zero) {
    digitalWrite(TRIAC_PIN, 0);
  } else {
    delayMicroseconds(dimmerPower);
    digitalWrite(TRIAC_PIN, 1);
    delayMicroseconds(MIN_MICROS);
    digitalWrite(TRIAC_PIN, 0);
  }
}

// Bresenham dimmer
void IRAM_ATTR ISR_Bresenham() {
  int val = ((uint16_t)++count * dimmerPower) >> 8;		
  if (lastVal != (val != last)) {
    digitalWrite(TRIAC_PIN, val != last);
  }
  lastVal = (val != last);
  last = val;
}

void setup_dimmer() {
  pinMode(TRIAC_PIN, OUTPUT);
  pinMode(ZCV_PIN, INPUT_PULLUP);

  // dimmer interrupt
  attachInterrupt(ZCV_PIN, ISR_PHASE, RISING);
}

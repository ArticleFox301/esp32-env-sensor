#include "sensors.h"

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN 35
#define MQ135_PIN 34

void sensors_init() {
  dht.begin();
}

float readTemperature() {
  return dht.readTemperature();
}

float readHumidity() {
  return dht.readHumidity();
}

int readLight() {
  int raw = analogRead(LDR_PIN);
  return map(4095 - raw, 0, 4095, 0, 100);
}

int readAirQuality() {
  int raw = analogRead(MQ135_PIN);
  return map(raw, 0, 4095, 0, 100);
}

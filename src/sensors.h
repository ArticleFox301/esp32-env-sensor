#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "DHT.h"

void sensors_init();
float readTemperature();
float readHumidity();
int readLight();
int readAirQuality();

#endif

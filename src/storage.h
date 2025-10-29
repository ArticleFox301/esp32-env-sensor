#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

void storage_init();
void appendJSON(float temp, float hum, int light, int air);

#endif

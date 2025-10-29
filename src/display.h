#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void display_init();
void display_show(float temp, float hum, int light, int air);

#endif

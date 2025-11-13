#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <Arduino.h>

// ThingSpeak settings - update these with your channel details in thingspeak.cpp
extern const char* THINGSPEAK_API_KEY;      // Your Write API Key - update in thingspeak.cpp
extern const unsigned long THINGSPEAK_CHANNEL_ID;  // Your Channel ID - update in thingspeak.cpp

void thingspeak_init();
bool sendToThingSpeak(float temp, float hum, int light, int air);

#endif
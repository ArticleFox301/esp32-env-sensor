#ifndef FIREBASE_RTDB_H
#define FIREBASE_RTDB_H

#include <Arduino.h>
#include "config.h"

// Initialize Firebase connection
void firebase_init();

// Send sensor data to Firebase
void firebase_send_data(float temp, float hum, int light, int air);

// Store historical sensor data
void firebase_store_historical_data(float temp, float hum, int light, int air);

// Handle background Firebase operations
void firebase_loop();

#endif
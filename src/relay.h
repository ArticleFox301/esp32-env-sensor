#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

// Initialize relay pins
void relay_init();

// Control individual relays
void relay1_on();
void relay1_off();
void relay2_on();
void relay2_off();

// Get relay states
bool relay1_state();
bool relay2_state();

// Toggle relay states
void relay1_toggle();
void relay2_toggle();

// Set both relays at once
void relay_set_states(bool relay1_state, bool relay2_state);

// Diagnostic function to test relay operation
void relay_test_sequence();

#endif
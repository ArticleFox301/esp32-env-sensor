#include "relay.h"
#include "pins.h"

static bool relay1_current_state = false;
static bool relay2_current_state = false;

void relay_init() {
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  
  // Initialize relays in OFF state (HIGH for most relay modules)
  digitalWrite(RELAY1, HIGH);  // Assuming active LOW relay
  digitalWrite(RELAY2, HIGH);  // Assuming active LOW relay
  
  relay1_current_state = false;
  relay2_current_state = false;
}

void relay1_on() {
  digitalWrite(RELAY1, LOW);  // Activate relay (assuming active LOW)
  relay1_current_state = true;
}

void relay1_off() {
  digitalWrite(RELAY1, HIGH); // Deactivate relay
  relay1_current_state = false;
}

void relay2_on() {
  digitalWrite(RELAY2, LOW);  // Activate relay (assuming active LOW)
  relay2_current_state = true;
}

void relay2_off() {
  digitalWrite(RELAY2, HIGH); // Deactivate relay
  relay2_current_state = false;
}

bool relay1_state() {
  return relay1_current_state;
}

bool relay2_state() {
  return relay2_current_state;
}

void relay1_toggle() {
  if (relay1_current_state) {
    relay1_off();
  } else {
    relay1_on();
  }
}

void relay2_toggle() {
  if (relay2_current_state) {
    relay2_off();
  } else {
    relay2_on();
  }
}

void relay_set_states(bool relay1_state, bool relay2_state) {
  if (relay1_state) {
    relay1_on();
  } else {
    relay1_off();
  }
  
  if (relay2_state) {
    relay2_on();
  } else {
    relay2_off();
  }
}
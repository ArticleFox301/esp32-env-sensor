#include "relay.h"
#include "pins.h"
#include <Arduino.h>  // Include for Serial debugging

static bool relay1_current_state = false;
static bool relay2_current_state = false;

void relay_init() {
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  
#ifdef RELAY_ACTIVE_HIGH
  // For active HIGH relays: writing HIGH turns the relay ON
  // So to start in OFF state, we write LOW initially
  digitalWrite(RELAY1, LOW);  // Turn OFF relay 1 initially (active HIGH)
  digitalWrite(RELAY2, LOW);  // Turn OFF relay 2 initially (active HIGH)
  Serial.println("Relays initialized as ACTIVE HIGH - GPIO " + String(RELAY1) + " and " + String(RELAY2));
#else
  // For active LOW relays: writing LOW turns the relay ON
  // So to start in OFF state, we write HIGH initially
  digitalWrite(RELAY1, HIGH);  // Turn OFF relay 1 initially (active LOW)
  digitalWrite(RELAY2, HIGH);  // Turn OFF relay 2 initially (active LOW)
  Serial.println("Relays initialized as ACTIVE LOW - GPIO " + String(RELAY1) + " and " + String(RELAY2));
#endif
  
  relay1_current_state = false;
  relay2_current_state = false;
  
  Serial.println("Relay 1 state: " + String(relay1_current_state ? "ON" : "OFF"));
  Serial.println("Relay 2 state: " + String(relay2_current_state ? "ON" : "OFF"));
}

void relay1_on() {
#ifdef RELAY_ACTIVE_HIGH
  // For active HIGH relays: writing HIGH turns the relay ON
  digitalWrite(RELAY1, HIGH);
  Serial.println("Relay 1 ON - GPIO " + String(RELAY1) + " set HIGH (Active HIGH)");
#else
  // For active LOW relays: writing LOW turns the relay ON
  digitalWrite(RELAY1, LOW);
  Serial.println("Relay 1 ON - GPIO " + String(RELAY1) + " set LOW (Active LOW)");
#endif
  relay1_current_state = true;
  
  // Additional debugging: Read back the pin state
  int pinState = digitalRead(RELAY1);
  Serial.println("DEBUG: Relay 1 pin " + String(RELAY1) + " state after ON command: " + String(pinState));
}

void relay1_off() {
#ifdef RELAY_ACTIVE_HIGH
  // For active HIGH relays: writing LOW turns the relay OFF
  digitalWrite(RELAY1, LOW);
  Serial.println("Relay 1 OFF - GPIO " + String(RELAY1) + " set LOW (Active HIGH)");
#else
  // For active LOW relays: writing HIGH turns the relay OFF
  digitalWrite(RELAY1, HIGH);
  Serial.println("Relay 1 OFF - GPIO " + String(RELAY1) + " set HIGH (Active LOW)");
#endif
  relay1_current_state = false;
  
  // Additional debugging: Read back the pin state
  int pinState = digitalRead(RELAY1);
  Serial.println("DEBUG: Relay 1 pin " + String(RELAY1) + " state after OFF command: " + String(pinState));
}

void relay2_on() {
#ifdef RELAY_ACTIVE_HIGH
  // For active HIGH relays: writing HIGH turns the relay ON
  digitalWrite(RELAY2, HIGH);
  Serial.println("Relay 2 ON - GPIO " + String(RELAY2) + " set HIGH (Active HIGH)");
#else
  // For active LOW relays: writing LOW turns the relay ON
  digitalWrite(RELAY2, LOW);
  Serial.println("Relay 2 ON - GPIO " + String(RELAY2) + " set LOW (Active LOW)");
#endif
  relay2_current_state = true;
  
  // Additional debugging: Read back the pin state
  int pinState = digitalRead(RELAY2);
  Serial.println("DEBUG: Relay 2 pin " + String(RELAY2) + " state after ON command: " + String(pinState));
}

void relay2_off() {
#ifdef RELAY_ACTIVE_HIGH
  // For active HIGH relays: writing LOW turns the relay OFF
  digitalWrite(RELAY2, LOW);
  Serial.println("Relay 2 OFF - GPIO " + String(RELAY2) + " set LOW (Active HIGH)");
#else
  // For active LOW relays: writing HIGH turns the relay OFF
  digitalWrite(RELAY2, HIGH);
  Serial.println("Relay 2 OFF - GPIO " + String(RELAY2) + " set HIGH (Active LOW)");
#endif
  relay2_current_state = false;
  
  // Additional debugging: Read back the pin state
  int pinState = digitalRead(RELAY2);
  Serial.println("DEBUG: Relay 2 pin " + String(RELAY2) + " state after OFF command: " + String(pinState));
}

void relay_test_sequence() {
  Serial.println("Starting relay test sequence...");
  
  // Test Relay 1
  Serial.println("Testing Relay 1...");
  relay1_on();
  delay(1000);
  relay1_off();
  delay(1000);
  
  // Test Relay 2
  Serial.println("Testing Relay 2...");
  relay2_on();
  delay(1000);
  relay2_off();
  delay(1000);
  
  // Test both relays on together
  Serial.println("Testing both relays ON...");
  relay1_on();
  relay2_on();
  delay(1000);
  relay1_off();
  relay2_off();
  
  Serial.println("Relay test sequence completed.");
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
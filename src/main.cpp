#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "sensors.h"
#include "display.h"
#include "storage.h"
#include "alert.h"
#include "thingspeak.h"
#include "relay.h"
#include "mqtt.h"
#include "globals.h"
#include "firebase_rtdb.h"

// Global variable to track manual/auto mode
bool manualMode = true; // Start in manual mode by default


const char* ssid = "P 2908";
const char* password = "@dimuonnoi@";


void setup() {
  Serial.begin(115200);


  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS failed");
    return;
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  sensors_init();
  display_init();
  storage_init();
  alert_init();
  thingspeak_init();
  relay_init();  // Initialize relays
  mqtt_init();   // Initialize MQTT
  firebase_init(); // Initialize Firebase

  Serial.println("✅ Hệ thống sẵn sàng");

  // Publish initial relay status after everything is initialized
  mqtt_publish_relay_status();
}

void loop() {
  mqtt_loop();  // Handle MQTT operations
  firebase_loop(); // Handle Firebase operations

  float t = readTemperature();
  float h = readHumidity();
  int l = readLight();
  int aq = readAirQuality();

  if (isnan(t) || isnan(h)) {
    Serial.println("⚠️ Lỗi đọc DHT22!");
  } else {
    display_show(t, h, l, aq);
    appendJSON(t, h, l, aq);

    // Send data to ThingSpeak every 20 seconds (to respect API limits)
    static unsigned long lastThingSpeakUpdate = 0;
    if (millis() - lastThingSpeakUpdate > 20000) { // 20 seconds
      sendToThingSpeak(t, h, l, aq);
      lastThingSpeakUpdate = millis();
    }

    // Publish data to MQTT (real-time)
    mqtt_publish_sensor_data(t, h, l, aq);

    // Send data to Firebase (real-time)
    firebase_send_data(t, h, l, aq);
  }
  alert_light(l);
  alert_air(aq);

  // In AUTO mode, control relays based on sensor values
  if (!manualMode) {  // auto mode is active
    // New auto logic as requested:
    // If temperature > 27°C, turn on relay 1
    // If light < 40, turn on relay 2
    // Otherwise, both relays turn off
    if (t > 27.0) {
      relay1_on();  // Turn on relay 1 if temperature > 27°C
    } else {
      relay1_off(); // Turn off relay 1 otherwise
    }

    if (l < 40) {  // If light level < 40
      relay2_on();  // Turn on relay 2
    } else {
      relay2_off(); // Turn off relay 2 otherwise
    }
  }

  delay(5000);
}

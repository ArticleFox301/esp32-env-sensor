#include "thingspeak.h"
#include "ThingSpeak.h"  // Include the ThingSpeak library
#include <WiFi.h>

// ThingSpeak settings - update these with your channel details
const char* THINGSPEAK_API_KEY = "LJ1XS8V5WQHFYUIL";  // Replace with your actual Write API Key
const unsigned long THINGSPEAK_CHANNEL_ID = 3157040;         // Replace with your actual Channel ID
// Note: If you want to use your specific values, replace:
// - YOUR_WRITE_API_KEY_HERE with: LJ1XS8V5WQHFYUIL
// - 0000000 with: 3157040

// WiFiClient for ThingSpeak
static WiFiClient client;

// Initialize ThingSpeak
void thingspeak_init() {
  ThingSpeak.begin(client);  // Initialize ThingSpeak with WiFi client
  Serial.println("ThingSpeak initialized");
}

// Send sensor data to ThingSpeak
bool sendToThingSpeak(float temp, float hum, int light, int air) {
  // Check if we're connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi not connected. Cannot send data to ThingSpeak.");
    return false;
  }

  // Set the fields with the sensor values
  ThingSpeak.setField(1, temp);      // Field 1: Temperature
  ThingSpeak.setField(2, hum);       // Field 2: Humidity
  ThingSpeak.setField(3, light);     // Field 3: Light level
  ThingSpeak.setField(4, air);       // Field 4: Air Quality

  // Write the data to your channel
  int responseCode = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
  
  if (responseCode == 200) {
    Serial.println("✅ Data successfully sent to ThingSpeak");
    Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%, Light: %d%%, Air Quality: %d%%\n", temp, hum, light, air);
    return true;
  } else {
    Serial.printf("❌ Error sending data to ThingSpeak. Response code: %d\n", responseCode);
    return false;
  }
}
#include "Firebase_ESP_Client.h"
#include <ArduinoJson.h>
#include "sensors.h"
#include "relay.h"
#include "config.h"
#include "firebase_rtdb.h"
#include <WiFi.h>
#include <time.h>

// Firebase Data object for RTDB operations
FirebaseData fbdo;

// Authentication configuration
FirebaseAuth auth;
FirebaseConfig config;

// Device ID
String deviceId = "";

void firebase_init() {
  Serial.println("Initializing Firebase...");

  // Get device ID
  deviceId = WiFi.macAddress();

  // Configure time for Vietnam (UTC+7)
  configTime(7 * 3600, 0, "pool.ntp.org"); // UTC+7 for Vietnam

  // Set up Firebase configuration
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&config, &auth);

  Serial.println("Firebase initialized");
}

void firebase_store_historical_data(float temp, float hum, int light, int air) {
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready for historical data storage");
    return;
  }

  Serial.println("Storing historical data to Firebase...");

  // Get current time for historical timestamp
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time for historical data");
    return;
  }

  // Create timestamp strings
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);

  char timeStr[9];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  Serial.printf("Storing historical data - Time: %s, Temp: %.2f, Hum: %.2f, Light: %d, Air: %d\n",
                timestamp, temp, hum, light, air);

  // Create JSON object for historical data
  FirebaseJson historyData;
  historyData.add("temperature", temp);
  historyData.add("humidity", hum);
  historyData.add("light", light);
  historyData.add("airQuality", air);
  historyData.add("timestamp", String(timestamp));
  historyData.add("device_id", deviceId);

  // Send to Firebase using the history structure
  String historyPath = "/sensor_history/" + deviceId + "/" + String(dateStr) + "/" + String(timeStr);
  Serial.println("Storing historical data to: " + historyPath);
  if (Firebase.RTDB.setJSON(&fbdo, historyPath.c_str(), &historyData)) {
    Serial.println("Historical data stored successfully: " + String(timestamp));
  } else {
    Serial.print("Failed to store historical data: ");
    Serial.println(fbdo.errorReason());
  }
}

void firebase_send_data(float temp, float hum, int light, int air) {
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready");
    return;
  }

  Serial.println("Sending sensor data to Firebase...");

  // Create JSON object for real-time data
  FirebaseJson jsonData;
  jsonData.add("temperature", temp);
  jsonData.add("humidity", hum);
  jsonData.add("light", light);
  jsonData.add("airQuality", air);
  jsonData.add("timestamp", millis());
  jsonData.add("device_id", deviceId);

  // Debug print
  Serial.printf("Sensor data - Temp: %.2f, Hum: %.2f, Light: %d, Air: %d\n", temp, hum, light, air);

  // Send to Firebase using correct API for real-time data
  String path = "/sensors/" + deviceId + "/data";
  Serial.println("Sending real-time data to: " + path);
  if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &jsonData)) {
    Serial.println("Real-time data sent to Firebase successfully");
  } else {
    Serial.print("Failed to send real-time data: ");
    Serial.println(fbdo.errorReason());
  }

  // Also store historical data
  Serial.println("Storing historical data...");
  firebase_store_historical_data(temp, hum, light, air);

  // Update device's registration in the devices path
  Serial.println("Updating device registration...");
  FirebaseJson deviceInfo;
  deviceInfo.add("name", String(DEVICE_NAME));
  deviceInfo.add("location", String(DEVICE_LOCATION));
  deviceInfo.add("macAddress", deviceId);
  deviceInfo.add("lastSeen", millis());
  deviceInfo.add("status", "online");
  deviceInfo.add("roomId", String(DEVICE_ROOM_ID)); // Use the configured room ID
  deviceInfo.add("firmwareVersion", "1.0.0");

  // Initialize sensor flags
  FirebaseJson sensors;
  sensors.add("temperature", true);
  sensors.add("humidity", true);
  sensors.add("light", true);
  sensors.add("airQuality", true);
  deviceInfo.add("sensors", sensors);

  String devicePath = "/devices/" + deviceId;
  Serial.println("Updating device info at: " + devicePath);
  if (Firebase.RTDB.setJSON(&fbdo, devicePath.c_str(), &deviceInfo)) {
    Serial.println("Device registered successfully in devices path");
  } else {
    Serial.print("Failed to register device: ");
    Serial.println(fbdo.errorReason());
  }

  Serial.println("All data sent to Firebase successfully!");
}

void firebase_loop() {
  // Handle any Firebase operations that need to run in loop
  // Currently not needed for simple data sending

  // Add periodic debug info if needed
  // Uncomment the following lines if you want to see periodic status
  // if (millis() % 30000 == 0) { // Every 30 seconds
  //   Serial.println("Firebase status: " + String(Firebase.ready() ? "Ready" : "Not ready"));
  // }
}
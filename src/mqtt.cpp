#include "mqtt.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "relay.h"  // Include relay control functions
#include "sensors.h" // Include for sensor data
extern bool manualMode; // Declare the manual mode variable

// MQTT settings for HiveMQ public broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_client_id = "esp32_sensor_client";
const char* mqtt_topic_sensor_data = "esp32/sensors/data";
const char* mqtt_topic_relay_control = "esp32/relays/control";
const char* mqtt_topic_relay_status = "esp32/relays/status";
const char* mqtt_topic_system_status = "esp32/system/status";

WiFiClient mqttEspClient;
PubSubClient mqttClient(mqttEspClient);

void mqtt_init() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqtt_callback);
  Serial.println("MQTT initialized");
}

void mqtt_connect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = mqtt_client_id;
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("✅ MQTT connected");
      // Once connected, publish an announcement...
      String message = "ESP32 sensor client connected - " + WiFi.localIP().toString();
      mqttClient.publish("esp32/status", message.c_str());
      // ... and resubscribe
      mqttClient.subscribe(mqtt_topic_relay_control);
      Serial.println("Subscribed to relay control topic: " + String(mqtt_topic_relay_control));
      // Publish initial relay status
      mqtt_publish_relay_status();
    } else {
      Serial.print("❌ failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool mqtt_publish_sensor_data(float temp, float hum, int light, int air) {
  if (!mqttClient.connected()) {
    mqtt_connect();
  }
  
  // Create JSON payload
  DynamicJsonDocument doc(256);
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["light"] = light;
  doc["air_quality"] = air;
  doc["timestamp"] = millis();
  doc["device_id"] = WiFi.macAddress();

  // Serialize JSON to string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Publish the sensor data
  bool result = mqttClient.publish(mqtt_topic_sensor_data, jsonString.c_str(), true); // retain message
  
  if (result) {
    Serial.println("✅ Sensor data published to MQTT: " + jsonString);
  } else {
    Serial.println("❌ Failed to publish sensor data to MQTT");
  }
  
  return result;
}

bool mqtt_publish_relay_status() {
  if (!mqttClient.connected()) {
    mqtt_connect();
  }
  
  // Create JSON payload with relay states
  DynamicJsonDocument doc(256);
  doc["relay1_state"] = relay1_state();
  doc["relay2_state"] = relay2_state();
  doc["manual_mode"] = manualMode; // Send actual manual mode value (not inverted)
  doc["timestamp"] = millis();
  doc["device_id"] = WiFi.macAddress();

  // Serialize JSON to string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Publish the relay status
  bool result = mqttClient.publish(mqtt_topic_relay_status, jsonString.c_str(), true); // retain message
  
  if (result) {
    Serial.println("✅ Relay status published to MQTT: " + jsonString);
  } else {
    Serial.println("❌ Failed to publish relay status to MQTT");
  }
  
  return result;
}

void mqtt_loop() {
  if (!mqttClient.connected()) {
    mqtt_connect();
  }
  mqttClient.loop();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ["); 
  Serial.print(topic);
  Serial.print("] ");
  
  // Convert payload to string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println(message);

  String topicStr = String(topic);
  
  // Handle relay control messages
  if (topicStr == mqtt_topic_relay_control) {
    // Parse the JSON command
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
      Serial.print("❌ JSON parsing failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    // Extract relay control parameters
    if (doc.containsKey("relay") && doc.containsKey("state")) {
      int relay = doc["relay"];
      bool state = doc["state"];
      
      if (relay == 1) {
        if (state) {
          relay1_on();
          Serial.println("Relay 1 turned ON via MQTT");
        } else {
          relay1_off();
          Serial.println("Relay 1 turned OFF via MQTT");
        }
      } else if (relay == 2) {
        if (state) {
          relay2_on();
          Serial.println("Relay 2 turned ON via MQTT");
        } else {
          relay2_off();
          Serial.println("Relay 2 turned OFF via MQTT");
        }
      }
      
      // Publish updated relay status after processing the command
      mqtt_publish_relay_status();
    }
    // Handle mode control commands
    else if (doc.containsKey("mode")) {
      String mode = doc["mode"];
      if (mode == "auto" || mode == "1" || mode == "true") {
        manualMode = false;
        Serial.println("Switched to AUTO mode via MQTT");
      } else {
        manualMode = true;
        Serial.println("Switched to MANUAL mode via MQTT");
      }
      // Publish updated status after mode change
      mqtt_publish_relay_status();
    }
    // Handle status request
    else if (doc.containsKey("request") && doc["request"] == "status") {
      mqtt_publish_relay_status();
    }
  }
}
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include "sensors.h"
#include "display.h"
#include "storage.h"
#include "alert.h"
#include "thingspeak.h"
#include "relay.h"


const char* ssid = "P 2908";
const char* password = "@dimuonnoi@";

WebServer server(80);

void handleLog() {
  File file = SPIFFS.open("/log.json", "r");
  if (!file) {
    server.send(404, "text/plain", "Log file not found");
    return;
  }

  String content = file.readString();
  file.close();
  server.send(200, "application/json", content);
}

void handleLogHtml() {
  File file = SPIFFS.open("/log.json", "r");
  if (!file) {
    server.send(404, "text/plain", "Log không tồn tại!");
    return;
  }

  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<title>Lịch sử cảm biến</title></head><body><h2>Lịch sử ghi log</h2><pre>";
  html += file.readString();
  html += "</pre></body></html>";

  file.close();
  server.send(200, "text/html", html);
}

void handleRelay() {
  if (server.hasArg("relay") && server.hasArg("action")) {
    String relay = server.arg("relay");
    String action = server.arg("action");
    
    if (relay == "1") {
      if (action == "on") {
        relay1_on();
        server.send(200, "text/plain", "Relay 1 ON");
      } else if (action == "off") {
        relay1_off();
        server.send(200, "text/plain", "Relay 1 OFF");
      } else if (action == "toggle") {
        relay1_toggle();
        String response = "Relay 1 toggled. Current state: ";
        response += (relay1_state()) ? "ON" : "OFF";
        server.send(200, "text/plain", response);
      } else {
        server.send(400, "text/plain", "Invalid action. Use 'on', 'off', or 'toggle'");
      }
    } else if (relay == "2") {
      if (action == "on") {
        relay2_on();
        server.send(200, "text/plain", "Relay 2 ON");
      } else if (action == "off") {
        relay2_off();
        server.send(200, "text/plain", "Relay 2 OFF");
      } else if (action == "toggle") {
        relay2_toggle();
        String response = "Relay 2 toggled. Current state: ";
        response += (relay2_state()) ? "ON" : "OFF";
        server.send(200, "text/plain", response);
      } else {
        server.send(400, "text/plain", "Invalid action. Use 'on', 'off', or 'toggle'");
      }
    } else {
      server.send(400, "text/plain", "Invalid relay. Use '1' or '2'");
    }
  } else {
    String response = "Current states - Relay 1: ";
    response += (relay1_state()) ? "ON" : "OFF";
    response += ", Relay 2: ";
    response += (relay2_state()) ? "ON" : "OFF";
    server.send(200, "text/plain", response);
  }
}

void handleRelayBoth() {
  if (server.hasArg("relay1") && server.hasArg("relay2")) {
    String r1 = server.arg("relay1");
    String r2 = server.arg("relay2");
    
    bool state1 = (r1 == "on" || r1 == "1" || r1 == "true") ? true : false;
    bool state2 = (r2 == "on" || r2 == "1" || r2 == "true") ? true : false;
    
    relay_set_states(state1, state2);
    
    String response = "Relays set - Relay 1: ";
    response += state1 ? "ON" : "OFF";
    response += ", Relay 2: ";
    response += state2 ? "ON" : "OFF";
    server.send(200, "text/plain", response);
  } else {
    server.send(400, "text/plain", "Missing arguments. Use ?relay1=on&relay2=off");
  }
}


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

  server.on("/log", handleLog);
  server.on("/log_html", handleLogHtml);
  server.on("/relay", handleRelay);
  server.on("/relay/both", handleRelayBoth);

  server.begin();
  Serial.println("Web server started!");
  sensors_init();
  display_init();
  storage_init();
  alert_init();
  thingspeak_init();
  relay_init();  // Initialize relays

  Serial.println("✅ Hệ thống sẵn sàng");
}

void loop() {
  server.handleClient();

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
  }
  alert_light(l);
  alert_air(aq);

  delay(5000);
}

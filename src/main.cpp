#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include "sensors.h"
#include "display.h"
#include "storage.h"

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

  server.begin();
  Serial.println("Web server started!");
  sensors_init();
  display_init();
  storage_init();

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

  }
  
  delay(5000);
}

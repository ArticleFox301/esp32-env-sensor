#include "storage.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

void storage_init() {
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS Error!");
    while (true);
  }
}

void appendJSON(float temp, float hum, int light, int air) {

  StaticJsonDocument<512> doc;     // Tạm dùng để đọc file hiện tại
  StaticJsonDocument<256> entry;   // Một mẫu dữ liệu mới

  entry["temperature"] = temp;
  entry["humidity"] = hum;
  entry["light"] = light;
  entry["air_quality"] = air;
  entry["timestamp"] = millis();

  // --- Đọc file nếu tồn tại ---
  if (SPIFFS.exists("/log.json")) {
    File file = SPIFFS.open("/log.json", FILE_READ);
    if (file) {
      DeserializationError err = deserializeJson(doc, file);
      file.close();
      if (err) {
        Serial.println("⚠️ File hỏng, tạo mới!");
        doc.to<JsonArray>(); // Reset thành mảng rỗng
      }
    }
  } else {
    doc.to<JsonArray>(); // Nếu file chưa có → tạo mảng mới
  }

  // --- Thêm entry mới vào mảng ---
  JsonArray arr = doc.as<JsonArray>();
  arr.add(entry);

  // --- Nếu log quá dài → giới hạn 200 mẫu ---
  if (arr.size() > 200) {
    arr.remove(0); // bỏ mẫu cũ nhất → tránh đầy SPIFFS
  }

  // --- Ghi lại file ---
  File file = SPIFFS.open("/log.json", FILE_WRITE);
  if (!file) {
    Serial.println("❌ Không thể mở /log.json để ghi!");
    return;
  }

  serializeJsonPretty(doc, file);
  file.close();

  Serial.println("✅ Đã ghi thêm log:");
  serializeJsonPretty(entry, Serial);
  Serial.println();
}

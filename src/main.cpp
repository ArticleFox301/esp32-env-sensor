#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "DHT.h"

// --- Cảm biến DHT22 ---
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Cảm biến ánh sáng (LDR) ---
#define LDR_PIN 35

// --- Cảm biến MQ-135 ---
#define MQ135_PIN 34

// --- Hàm hiển thị LCD ---
void hienThiLCD(float nhietDo, float doAm, int anhSang, int chatLuong) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(nhietDo, 1);
  lcd.print((char)223); // ký hiệu °
  lcd.print("C H:");
  lcd.print(doAm, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("AS:");
  lcd.print(anhSang);
  lcd.print("% AQ:");
  lcd.print(chatLuong);
  lcd.print("%");
}

// --- Hàm ghi JSON vào SPIFFS ---
void luuJSON(float nhietDo, float doAm, int anhSang, int chatLuong) {
  StaticJsonDocument<256> doc;

  doc["temperature"] = nhietDo;
  doc["humidity"] = doAm;
  doc["light"] = anhSang;
  doc["air_quality"] = chatLuong;
  doc["timestamp"] = millis();

  File file = SPIFFS.open("/data.json", FILE_WRITE);
  if (!file) {
    Serial.println("⚠️ Không thể mở file để ghi!");
    return;
  }

  // Ghi JSON dạng đẹp (dễ đọc)
  serializeJsonPretty(doc, file);
  file.close();

  Serial.println("✅ Đã ghi dữ liệu vào /data.json:");
  serializeJsonPretty(doc, Serial);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("Khoi dong...");
  delay(1500);
  lcd.clear();

  // --- Khởi tạo SPIFFS ---
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Lỗi khởi tạo SPIFFS!");
    lcd.print("SPIFFS error!");
    while (true);
  } else {
    Serial.println("✅ SPIFFS đã sẵn sàng.");
  }
}

void loop() {
  // ---- Đọc dữ liệu cảm biến ----
  float nhietDo = dht.readTemperature();
  float doAm = dht.readHumidity();
  int asRaw = analogRead(LDR_PIN);
  int mqRaw = analogRead(MQ135_PIN);

  // ---- Chuyển đổi giá trị ----
  int anhSang = map(4095 - asRaw, 0, 4095, 0, 100);
  int chatLuong = map(mqRaw, 0, 4095, 0, 100);

  // ---- Kiểm tra lỗi ----
  if (isnan(nhietDo) || isnan(doAm)) {
    Serial.println("⚠️ Lỗi đọc DHT22!");
    lcd.clear();
    lcd.print("Loi doc DHT22");
    delay(2000);
    return;
  }

  // ---- In ra Serial ----
  Serial.print("🌡 Nhiet do: ");
  Serial.print(nhietDo);
  Serial.print("°C | 💧 Do am: ");
  Serial.print(doAm);
  Serial.print("% | ☀️ Anh sang: ");
  Serial.print(anhSang);
  Serial.print("% | 🏭 MQ135: ");
  Serial.print(chatLuong);
  Serial.println("%");

  // ---- Hiển thị lên LCD ----
  hienThiLCD(nhietDo, doAm, anhSang, chatLuong);

  // ---- Lưu JSON vào SPIFFS ----
  luuJSON(nhietDo, doAm, anhSang, chatLuong);

  delay(5000); // lưu mỗi 5 giây
}

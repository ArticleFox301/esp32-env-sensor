#include <Arduino.h>
#include "pins.h"

void alert_init() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Tắt toàn bộ lúc khởi động
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(BUZZER, LOW);
}

void alert_light(int light) {
  // Cảnh báo ánh sáng yếu (<20%)
  if (light < 20) {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  }
  // Ánh sáng bình thường (20% - 60%)
  else if (light < 60) {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  }
  // Ánh sáng quá mạnh (>60%)
  else {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, HIGH);
  }
}

void alert_air(int airQuality) {
  // Nếu chất lượng không khí xấu (>70%) → bật buzzer cảnh báo
  if (airQuality > 70) {
    tone(BUZZER, 800); // kêu 800Hz
  } else {
    noTone(BUZZER);
  }
}

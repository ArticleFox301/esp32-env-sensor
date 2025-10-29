#include "display.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void display_init() {
  lcd.init();
  lcd.backlight();
}

void display_show(float temp, float hum, int light, int air) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temp, 1); lcd.print((char)223); lcd.print("C H:"); lcd.print(hum, 0); lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("AS:"); lcd.print(light); lcd.print("% AQ:"); lcd.print(air); lcd.print("%");
}

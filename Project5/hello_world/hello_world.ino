#include <ESP32Time.h>
#include <LiquidCrystal_I2C.h>
// Initialize the LCD object with the appropriate pins

LiquidCrystal_I2C lcd(0x27, 20, 4);
ESP32Time rtc(3600);
void setup() {
  lcd.begin(20, 4);  // Initialize the LCD with 16x2 characters
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello,");  // Print the first part of the message
  lcd.setCursor(0, 1);  // Move to the second row
  lcd.print("World!");  // Print the second part of the message
  rtc.setTime(0, 0, 23, 12, 6, 2023);
}

void loop() {
  // Nothing to do here for a static display
  lcd.setCursor(10, 1);
  lcd.print(rtc.getTime());
}

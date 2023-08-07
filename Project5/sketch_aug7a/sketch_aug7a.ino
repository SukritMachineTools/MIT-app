#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <RTClib.h>

RTC_DS3231 rtc;

LiquidCrystal_I2C lcd(0x27, 20, 4);

int counter1 = 0;
unsigned long p_on_time = 0;
unsigned long prod_time = 0;
unsigned long n_prod_time = 0;
unsigned long var1 = 0;
unsigned long cycleStartTime = 0;
unsigned long cycleTime = 0;

int optoPin2 = 2;
int optoPinrst = 14;
int optoPin2State = LOW;
int lastoptoPin2State = LOW;

int counter1Address = 0;

AsyncWebServer server(80);

const char* ssid = "DeviceL1";
const char* password = "";

void handleRoot(AsyncWebServerRequest *request){
    String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(rtc.now().timestamp()) + "," + String(var1));
    request->send(200, "text/plain", message);
}

void setup() {
    pinMode(optoPin2, INPUT);
    pinMode(optoPinrst, INPUT);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server.on("/", HTTP_GET, handleRoot);
    server.begin();

    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    EEPROM.begin(sizeof(counter1) + sizeof(cycleTime));

    EEPROM.get(counter1Address, counter1);

    pinMode(optoPin2, INPUT_PULLUP);
    pinMode(optoPinrst, INPUT_PULLUP);

    lcd.begin(20, 4);
    lcd.init();
    lcd.backlight();
}

void loop() {
    optoPin2State = digitalRead(optoPin2);
    if (optoPin2State != lastoptoPin2State) {
        if (optoPin2State == LOW) {
            cycleStartTime = millis();
        }
        if (optoPin2State == HIGH) {
            cycleTime = millis() - cycleStartTime;
            cycleTime = cycleTime / 1000;
            var1 = cycleTime + var1;
            counter1 = counter1 + 1;
        }
        lastoptoPin2State = optoPin2State;
    }

    p_on_time = millis() / 60000;
    prod_time = var1 / 60;
    n_prod_time = p_on_time - prod_time;

    DateTime now = rtc.now();

    lcd.setCursor(0, 0);
    lcd.print(now.timestamp());

    lcd.setCursor(10, 0);
    lcd.print(p_on_time);

    lcd.setCursor(0, 2);
    lcd.print("P.T.: " + String(prod_time));

    lcd.setCursor(10, 2);
    lcd.print("NP.T.: " + String(n_prod_time));

    lcd.setCursor(0, 3);
    lcd.print(var1);

    if (digitalRead(optoPinrst) == LOW) {
        counter1 = 0;
        cycleTime = 0;
        EEPROM.put(counter1Address, counter1);
        EEPROM.commit();

        lcd.setCursor(0, 1);
        lcd.print("C.T.:" + String(cycleTime) + " ");

        lcd.setCursor(10, 1);
        lcd.print("PRT1:" + String(counter1) + " ");
    }

    delay(100); // Avoid excessive looping and reduce CPU usage
}

//libs
#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <ESP32Time.h>
////////////////////////////////////////////////////
ESP32Time rtc(3600);                 // offset in seconds GMT+1
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the I2C address to 0x27 and dimensions to 20x4

//////////////////////////////////////////////////////////////////////////
//variables
int counter1 = 0;  // Counter controlled by Button 1
int t_Start = 0;
int t_stop = 0;
int optoPin2 = 13;
int optoPinrst = 33;
// int optoPinm1 = 18;
int optoPinm1 = 27;
int optoPinm2 = 18;
int optoPinm3 = 26;
int optoPinm4 = 14;
int optoPinm5 = 25;

int optoPin2State = LOW;
int lastoptoPin2State = LOW;
int optoPinm1State = LOW;


unsigned long lastDebounceTime1 = 0;
unsigned long debounceDelay = 50;
unsigned long p_on_time = 0;
unsigned long prod_time = 0;
unsigned long n_prod_time = 0;
unsigned long eff_time = 0;
unsigned long var1 = 0;
unsigned long var2 = 0;

////////////////////////////////////////////////////////////////////////////////
//address

int counter1Address = 0;                  // Address in the EEPROM emulation to store the counter1 value
int cycleTimeAddress = sizeof(counter1);  // Address in the EEPROM emulation to store the cycle time value

unsigned long cycleStartTime = 0;  // To store the time when the button is pressed
unsigned long cycleTime = 0;       // To store the calculated cycle time
//--------------------------------------------------------------------------------------------------------------
//*by me
unsigned long newVar2 = 0;
// unsigned long var2 = 0;
unsigned long lastVar2Update = 0;
unsigned long var2UpdateInterval = 1000;
//------------------------------------------------------------------------------------------------------------
//my variables
String currentRtcTime;
int rtaddress = 10;
const int maxStringLength = 50;
unsigned long sec = 0;
unsigned long min1 = 0;
unsigned long hr = 0;
int rtcHourAddress = +sizeof(int);
int rtcMinuteAddress = rtcHourAddress + sizeof(int);
int rtcSecondAddress = rtcMinuteAddress + sizeof(int);
int storedHour, storedMinute, storedSecond;
int Ttimead = rtcSecondAddress + sizeof(int);
int Ctimeadd = Ttimead + sizeof(int);
int Prt1add = Ctimeadd + sizeof(int);
int PTadd = Prt1add + sizeof(int);
int NPTadd = PTadd + sizeof(int);
int var1add = NPTadd + sizeof(int);
int counter1add = var1add + sizeof(int);
unsigned long p_on_timeval, prod_timeval, n_prod_timeval, cycleTimeval, var1val, counter1val;
int milliFlag = 0;
unsigned long resetcycleStartTime = 0;
String myvar2;
int getsec, getmin, gethr;
unsigned long p_o = 0;
unsigned long p_oadd = counter1add + sizeof(unsigned long);
unsigned long p_min = 0;
unsigned long p_minadd = p_oadd + sizeof(unsigned long);
//------------------------------------------------------------------------------------------------------
//modes
unsigned long mode = 0;
int mtime = 0, setime = 0;
unsigned long mcycleStartTime = 0, scycleStartTime = 0;
unsigned long mcycleTime = 0, scycleTime = 0;
unsigned long mcycleadd = p_minadd + sizeof(unsigned long);
unsigned long scycleadd = mcycleadd + sizeof(unsigned long);
int pflag = 0, mflag = 0, sflag = 1;
static unsigned long mTime = 0, pTime = 0, sTime = 0;
static bool srunning = false, mrunning = false;
int mhr = 0, mmin = 0, shr = 0, smin = 0;
//--------------------------------------------------------------------------------------------------
IPAddress newip, newgip, newnmask;
int epip = 0;
const char* ssid = "DeviceL1";
const char* password = "";
IPAddress Ip(192, 168, 4, 10);
IPAddress GIp(192, 168, 4, 1);
IPAddress NMask(255, 255, 255, 0);
unsigned long Ipadd = scycleadd + sizeof(unsigned long);
unsigned long GIpadd = Ipadd + sizeof(unsigned long);
unsigned long NMaskadd = GIpadd + sizeof(unsigned long);
unsigned long ssidadd = NMaskadd + sizeof(unsigned long);
unsigned long passwordadd = ssidadd + sizeof(unsigned long);
unsigned long epipadd = passwordadd + sizeof(unsigned long);


const int relay2 = 19;  //relay2
const int relay1 = 23;  //relay1
////////////////////////////////////////////////////////////////////////////////////////////////
String emp = "     ";
AsyncWebServer server(80);
// const char* ssid = "DeviceL1";
// const char* password = "";
IPAddress ip;


/////////////////////////////////////////////////////////////////////////////////////////////////
void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void resettime() {
  //code
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void resetparams() {
  counter1 = 0;

  cycleTime = 0;

  /*my*/
  counter1val = 0;
  cycleTime = 0;
  var1 = 0;
  var1val = 0;
  resetcycleStartTime = millis();
  milliFlag = 1;
  p_o = 0;
  p_min = 0;
  mcycleTime = 0;
  scycleTime = 0;
  mmin = 0;
  smin = 0;

  lcd.setCursor(0, 1);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("C.T.:" + String(cycleTime) + "  ");

  lcd.setCursor(10, 1);
  lcd.print("   ");

  lcd.setCursor(10, 1);
  lcd.print("PRT1:" + String(counter1) + "  ");

  rtc.setTime(0, 0, 23, 12, 6, 2023);

  lcd.setCursor(10, 0);
  lcd.print(String(p_on_time) + " ");

  lcd.setCursor(0, 2);
  lcd.print("P.T.: " + String(prod_time) + " ");

  lcd.setCursor(10, 2);
  lcd.print("NP.T.: " + String(n_prod_time) + " ");

  lcd.setCursor(0, 3);
  lcd.print(String(var1) + " ");

  lcd.setCursor(15, 0);
  lcd.print(String(p_o) + " ");

  lcd.setCursor(17, 0);
  lcd.print(String(p_min) + " ");

  // EEPROM.write(mcycleadd, mmin);
  // EEPROM.write(scycleadd, smin);
  // EEPROM.write(p_oadd, p_o);
  // EEPROM.write(p_minadd, p_min);
  EEPROM.put(mcycleadd, mmin);  // Write the integer to the first four slots
  EEPROM.put(scycleadd, smin);  // Write the integer to the first four slots
  EEPROM.put(p_oadd, p_o);      // Write the integer to the first four slots
  EEPROM.put(p_minadd, p_min);  // Write the integer to the first four slots

  // Store the reset counter values in EEPROM

  EEPROM.put(counter1Address, counter1);

  EEPROM.commit();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  pinMode(optoPin2, INPUT);

  pinMode(optoPinrst, INPUT);
  pinMode(optoPinm1, INPUT);
  pinMode(optoPinm2, INPUT);
  pinMode(optoPinm3, INPUT);
  pinMode(optoPinm4, INPUT);
  pinMode(optoPinm5, INPUT);



  WiFi.mode(WIFI_AP);
  EEPROM.get(epipadd, epip);
  if (epip == 1) {
    EEPROM.get(ssidadd, ssid);
    EEPROM.get(passwordadd, password);
    String ip1, ip2, ip3;
    EEPROM.get(Ipadd, ip1);
    Ip.fromString(ip1);
    EEPROM.get(GIpadd, ip2);
    GIp.fromString(ip2);
    EEPROM.get(NMaskadd, ip3);
    NMask.fromString(ip3);
  }
  WiFi.softAP(ssid, password);
  delay(100);
  // IPAddress Ip(192, 168, 4, 10);
  // IPAddress GIp(192, 168, 4, 1);
  // IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, GIp, NMask);

  ip = WiFi.softAPIP();

  Serial.print("AP IP address: ");

  Serial.println(ip);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(var1) + "," + String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()) + "," + String(p_o) + "," + String(p_min) + "," + String(mmin) + "," + String(smin) + "," + String(mode));
    request->send(200, "text/plain", message);
  });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* j = request->getParam(0);
    String postBody = j->value();

    if (postBody == "reset") {
      AsyncWebParameter* k = request->getParam(1);
      getsec = (k->value()).toInt();
      AsyncWebParameter* l = request->getParam(2);
      getmin = (l->value()).toInt();
      AsyncWebParameter* m = request->getParam(3);
      gethr = (m->value()).toInt();
      rtc.setTime(getsec, getmin, (gethr - 1), 12, 6, 2023);
      AsyncWebParameter* n = request->getParam(4);
      p_o = (n->value()).toInt();
      AsyncWebParameter* o = request->getParam(5);
      p_min = (o->value()).toInt();
      request->send(200, "text/plain", "time reset successful");

      sec = getsec;
      min1 = getmin;
      hr = gethr;


      EEPROM.write(rtcHourAddress, hr);
      EEPROM.write(rtcMinuteAddress, min1);
      EEPROM.write(rtcSecondAddress, sec);
      // EEPROM.write(p_oadd, p_o);
      // EEPROM.write(p_minadd, p_min);
      EEPROM.put(p_oadd, p_o);      // Write the integer to the first four slots
      EEPROM.put(p_minadd, p_min);  // Write the integer to the first four slots
      EEPROM.commit();
    } else {
      request->send(400, "text/plain", "invalid action");
    }

    request->send(200);
  });
  //-----------------------------------------------------------------------------------------------------
  server.on("/admin", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* action = request->getParam(0);
    String postBody = action->value();

    if (postBody == "reset_admin") {
      AsyncWebParameter* new_ssid = request->getParam(1);
      String newssid = (new_ssid->value());
      AsyncWebParameter* new_password = request->getParam(2);
      String newpassword = (new_password->value());
      AsyncWebParameter* new_ip = request->getParam(3);
      newip.fromString(new_ip->value());

      AsyncWebParameter* new_gip = request->getParam(4);
      newgip.fromString(new_gip->value());
      AsyncWebParameter* new_nmask = request->getParam(5);
      newnmask.fromString(new_nmask->value());
      request->send(200, "text/plain", "time reset successful");

      EEPROM.put(ssidadd, String(newssid));
      EEPROM.put(passwordadd, String(newpassword));
      EEPROM.put(Ipadd, String(newip));
      EEPROM.put(GIpadd, String(newgip));
      EEPROM.put(NMaskadd, String(newnmask));
      epip = 1;
      EEPROM.put(epipadd, epip);
      EEPROM.commit();
      if (epip == 1) {
        WiFi.softAP(newssid, newpassword);
        delay(100);
        WiFi.softAPConfig(newip, newgip, newnmask);

        ip = WiFi.softAPIP();
      }
    } else {
      request->send(400, "text/plain", "invalid action");
    }

    request->send(200);
  });




  server.onNotFound(notFound);
  server.begin();


  Serial.println("Server started");

  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();


  rtc.setTime(0, 0, 23, 12, 6, 2023);

  // EEPROM.begin(sizeof(counter1) + sizeof(cycleTime));  // Initialize EEPROM with the desired data size
  EEPROM.begin(512);

  // Read the counter and cycle time values from EEPROM during initialization

  EEPROM.get(counter1Address, counter1);

  EEPROM.get(cycleTimeAddress, cycleTime);



  if (counter1 < 0 || counter1 > 9999) {
    // Invalid counter1 value stored in EEPROM, reset to zero

    counter1 = 0;
  }

  //pinMode(button1Pin, INPUT_PULLUP);

  pinMode(optoPin2, INPUT_PULLUP);



  //pinMode(resetButtonPin, INPUT_PULLUP);

  pinMode(optoPinrst, INPUT_PULLUP);
  pinMode(optoPinm1, INPUT_PULLUP);
  pinMode(optoPinm2, INPUT_PULLUP);
  pinMode(optoPinm3, INPUT_PULLUP);
  pinMode(optoPinm4, INPUT_PULLUP);
  pinMode(optoPinm5, INPUT_PULLUP);

  //pinMode(ledPin, OUTPUT);

  storedHour = int(EEPROM.read(rtcHourAddress));
  storedMinute = int(EEPROM.read(rtcMinuteAddress));
  storedSecond = int(EEPROM.read(rtcSecondAddress));
  rtc.setTime(storedSecond, storedMinute, storedHour - 1, 12, 6, 2023);
  myvar2 = String(storedHour) + ":" + String(storedMinute) + ":" + String(storedSecond);
  lcd.setCursor(3, 3);
  lcd.print(myvar2);

  // p_on_timeval = EEPROM.read(Ttimead);

  EEPROM.get(Ttimead, p_on_timeval);     // Read the integer from EEPROM
  EEPROM.get(PTadd, prod_timeval);       // Read the integer from EEPROM
  EEPROM.get(NPTadd, n_prod_timeval);    // Read the integer from EEPROM
  EEPROM.get(Ctimeadd, cycleTimeval);    // Read the integer from EEPROM
  EEPROM.get(var1add, var1val);          // Read the integer from EEPROM
  EEPROM.get(counter1add, counter1val);  // Read the integer from EEPROM
  EEPROM.get(p_oadd, p_o);               // Read the integer from EEPROM
  EEPROM.get(p_minadd, p_min);           // Read the integer from EEPROM
  EEPROM.get(mcycleadd, mmin);           // Read the integer from EEPROM
  EEPROM.get(scycleadd, smin);           // Read the integer from EEPROM

  if (counter1val <= 0) {
    counter1 = 0;
    counter1val = 0;
  } else {
    counter1 = counter1val - 1;
  }
  if (var1val <= 0) {
    var1 = 0;
    var1val = 0;
  } else {
    var1 = var1val - 2;
  }
  if (cycleTimeval <= 0) {
    cycleTime = 0;
    cycleTimeval = 0;
  } else {
    cycleTime = cycleTimeval - 2;
  }
  pinMode(relay2, OUTPUT);  //relay2
}
//////////////////////////////////////////////////////////////////////////////////////////////
void loop()

{
  if (digitalRead(optoPinm1) == LOW) {
    digitalWrite(relay2, HIGH);  //relay2
    mode = 2;
    mrunning = false;
    srunning = false;
    lcd.setCursor(17, 3);
    lcd.print(mode);
    lcd.setCursor(15, 0);

    lcd.print(p_o);

    lcd.setCursor(17, 0);

    lcd.print(p_min);
    lcd.setCursor(0, 1);

    lcd.print("C.T.:" + String(cycleTime));
    lcd.setCursor(10, 1);

    lcd.print("PRT1:" + String(counter1));

    optoPin2State = digitalRead(optoPin2);

    if (optoPin2State != lastoptoPin2State)

    {

      if (optoPin2State == LOW)

      {

        cycleStartTime = millis();
      }


      if (optoPin2State == HIGH)

      {
        lcd.setCursor(10, 1);

        lcd.print("PRT1:" + String(counter1));

        cycleTime = millis() - cycleStartTime;  // Calculate the cycle time

        cycleTime = (cycleTime / 1000);
        cycleTimeval = cycleTime;

        var1 = cycleTime + var1;
        var1val = var1;

        counter1 = counter1 + 1;
        counter1val = counter1;


        lcd.setCursor(0, 1);

        lcd.print("C.T.:" + String(cycleTime));
      }

      lastoptoPin2State = optoPin2State;
    }

    p_on_time = rtc.getMinute() + (int(rtc.getHour()) * 60);

    if (var1 == 0) {
      prod_time = 0;
    } else {
      prod_time = var1 / 60;
    }

    n_prod_time = p_on_time - prod_time;

    lcd.setCursor(0, 0);

    lcd.print(rtc.getTime());

    lcd.setCursor(10, 0);
    lcd.print(p_on_time);

    lcd.setCursor(0, 2);
    lcd.print("P.T.: " + String(prod_time));

    lcd.setCursor(10, 2);
    lcd.print("NP.T.: " + String(n_prod_time));

    lcd.setCursor(0, 3);
    lcd.print(var1);

    currentRtcTime = rtc.getTime();
    sec = rtc.getSecond();
    min1 = rtc.getMinute();
    hr = rtc.getHour();

    EEPROM.write(rtcHourAddress, hr);
    EEPROM.write(rtcMinuteAddress, min1);
    EEPROM.write(rtcSecondAddress, sec);

    // EEPROM.write(Ttimead, p_on_timeval);

    EEPROM.put(Ttimead, p_on_timeval);     // Write the integer to the first four slots
    EEPROM.put(PTadd, prod_timeval);       // Write the integer to the first four slots
    EEPROM.put(NPTadd, n_prod_timeval);    // Write the integer to the first four slots
    EEPROM.put(Ctimeadd, cycleTimeval);    // Write the integer to the first four slots
    EEPROM.put(var1add, var1val);          // Write the integer to the first four slots
    EEPROM.put(counter1add, counter1val);  // Write the integer to the first four slots
    EEPROM.put(p_oadd, p_o);               // Write the integer to the first four slots
    EEPROM.put(p_minadd, p_min);           // Write the integer to the first four slots

    EEPROM.commit();

    if (int(rtc.getHour()) >= 12) {
      resetparams();
    }


    if (digitalRead(optoPinrst) == LOW) {

      resetparams();
    }
  }  //mode1
  else if (digitalRead(optoPinm2) == LOW) {
    digitalWrite(relay2, HIGH);  //relay2
    //Maintence mode
    storedHour = int(EEPROM.read(rtcHourAddress));
    storedMinute = int(EEPROM.read(rtcMinuteAddress));
    storedSecond = int(EEPROM.read(rtcSecondAddress));
    rtc.setTime(storedSecond, storedMinute, storedHour - 1, 12, 6, 2023);
    myvar2 = String(storedHour) + ":" + String(storedMinute) + ":" + String(storedSecond);
    lcd.setCursor(3, 3);
    lcd.print(myvar2);
    mode = 1;
    lcd.setCursor(17, 3);
    lcd.print(mode);
    srunning = false;
    if (!mrunning) {
      mcycleStartTime = millis();
      mrunning = true;
    }

    if (mrunning) {
      mcycleTime = (millis() - mcycleStartTime) / 1000;
      if (mcycleTime == 0) {
        mhr = 0;
        mmin = 0;
      } else {
        mhr = mcycleTime / 3600;
        mmin = mcycleTime / 60;
      }
      lcd.setCursor(0, 0);
      lcd.print("Maintenance ;)");
      lcd.setCursor(0, 1);
      lcd.print(String(mhr) + ":" + String(mmin) + ":" + String(mcycleTime % 60));
      // EEPROM.write(mcycleadd, mmin);
      EEPROM.put(mcycleadd, mmin);  // Write the integer to the first four slots
      lcd.setCursor(10, 0);
      lcd.print(emp);
      lcd.setCursor(10, 1);
      lcd.print(emp);
      lcd.setCursor(10, 2);
      lcd.print(emp);
      lcd.setCursor(10, 3);
      lcd.print(emp);
      lcd.setCursor(0, 2);
      lcd.print(emp);
      // lcd.setCursor(0, 3);
      // lcd.print(emp);
    }  //mode2






  } else if (digitalRead(optoPinm3) == LOW) {
    digitalWrite(relay2, HIGH);  //relay2
    //setting mode
    storedHour = int(EEPROM.read(rtcHourAddress));
    storedMinute = int(EEPROM.read(rtcMinuteAddress));
    storedSecond = int(EEPROM.read(rtcSecondAddress));
    rtc.setTime(storedSecond, storedMinute, storedHour - 1, 12, 6, 2023);
    myvar2 = String(storedHour) + ":" + String(storedMinute) + ":" + String(storedSecond);
    lcd.setCursor(3, 3);
    lcd.print(myvar2);
    mode = 3;
    lcd.setCursor(17, 3);
    lcd.print(mode);
    mrunning = false;
    if (!srunning) {
      scycleStartTime = millis();
      srunning = true;
    }

    if (srunning) {
      scycleTime = (millis() - scycleStartTime) / 1000;
      if (scycleTime == 0) {
        smin = 0;
        shr = 0;
      } else {
        smin = scycleTime / 60;
        shr = scycleTime / 3600;
      }
      lcd.setCursor(0, 0);
      lcd.print("Setting Mode ;)");
      lcd.setCursor(0, 1);
      lcd.print(String(shr) + ":" + String(smin) + ":" + String(scycleTime % 60));
      // EEPROM.write(scycleadd, scycleTime);
      // EEPROM.write(scycleadd, smin);
      EEPROM.put(scycleadd, smin);  // Write the integer to the first four slots

      lcd.setCursor(10, 0);
      lcd.print(emp);
      lcd.setCursor(10, 1);
      lcd.print(emp);
      // lcd.setCursor(2, 1);
      // lcd.print(emp);
      lcd.setCursor(10, 2);
      lcd.print(emp);
      lcd.setCursor(10, 3);
      lcd.print(emp);
      lcd.setCursor(0, 2);
      lcd.print(emp);
      // lcd.setCursor(0, 3);
      // lcd.print(emp);
      //-----------------------------------------------
      if (epip == 1) {
        EEPROM.get(ssidadd, ssid);
        EEPROM.get(passwordadd, password);
        EEPROM.get(Ipadd, Ip);
        EEPROM.get(GIpadd, GIp);
        EEPROM.get(NMaskadd, NMask);
      }
      WiFi.softAP(ssid, password);
      delay(100);
      // IPAddress Ip(192, 168, 4, 10);
      // IPAddress GIp(192, 168, 4, 1);
      // IPAddress NMask(255, 255, 255, 0);
      WiFi.config(Ip, GIp, NMask);

      ip = WiFi.softAPIP();
    }

  } else if (digitalRead(optoPinm4) == LOW) {
    lcd.setCursor(2, 0);
    lcd.print(ip);
    lcd.setCursor(2, 1);
    lcd.print(WiFi.gatewayIP());
    lcd.setCursor(2, 2);
    lcd.print(WiFi.subnetMask());
  } else {
    // lcd.setCursor(0, 0);
    lcd.clear();
    srunning = false;
    mrunning = false;
    // lcd.print("Error :D");
  }
}
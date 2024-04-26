#include <Wire.h>
#include "RTC.h"
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#define SECRET_SSID "xxxxxxxxxxxxxxxxx"
#define SECRET_PASS "xxxxxxxxxxxxxxxxxxxxxxx"

#define BUTTON_PIN 2
#define WinterTimeOutPin 8
#define WinterTimeInputPin 9
#define randomInputPin A0
#define LED_COUNT 16
#define LED_PIN 3
#define MAX_PATTERN 9

int winterTime = 0;
RTCTime currentTime;
int lastMinute = -1;
int RTCMinute = -1;
int preDirection = 0;
int currentPattern = 0;
bool patternSelect = false;
unsigned long lastInterrupt = 0;
bool sleepFlag = false;

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp;  // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp, "pool.ntp.org", -7 * 3600, 10 * 60 * 1000);

char sendData[8] = { '0', '0', '0', '0', '0', '0', '0', '0' };
char digits[10][6][5][2] = {
  //0
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  //1
  { { { '3', '3' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '3', '3' } },
    { { '3', '3' }, { '0', 'c' }, { 'c', 'c' }, { '0', 'c' }, { '3', '3' } },
    { { '3', '3' }, { '0', 'c' }, { '0', 'c' }, { '0', 'c' }, { '3', '3' } },
    { { '3', '3' }, { '0', 'c' }, { '0', 'c' }, { '0', 'c' }, { '3', '3' } },
    { { '3', '3' }, { '0', 'c' }, { '0', '0' }, { '0', 'c' }, { '3', '3' } },
    { { '3', '3' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '3', '3' } } },
  //2
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  //3
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  //4
  { { { '6', 'c' }, { 'c', 'i' }, { '3', '3' }, { '6', 'c' }, { 'c', 'i' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '3', '3' }, { '3', '3' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' } },
    { { '3', '3' }, { '3', '3' }, { '3', '3' }, { '0', '6' }, { '0', 'i' } } },
  //5
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  //6
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  //7
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '3', '3' }, { '3', '3' }, { '3', '3' }, { '0', 'f' }, { '0', 'f' } },
    { { '3', '3' }, { '3', '3' }, { '3', 'c' }, { '3', 'c' }, { '3', '3' } },
    { { '3', '3' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' }, { '3', '3' } },
    { { '3', '3' }, { '3', '3' }, { '0', '6' }, { '0', 'i' }, { '3', '3' } } },
  //8
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  //9
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } }
};
char letters[4][6][4][2] = {
  /////////S///////////////
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '0', 'i' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '6', 'c' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  ///////////L///////////
  { { { '6', 'c' }, { 'c', 'i' }, { '3', '3' }, { '3', '3' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  /////////////////E///////////////
  { { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '0', 'i' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '0', 'i' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { 'c', 'i' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } } },
  /////////////P//////////
  { { { '6', 'c' }, { '6', 'i' }, { '9', 'i' }, { '3', '3' } },
    { { '0', 'c' }, { '6', 'c' }, { 'c', 'i' }, { 'c', 'l' } },
    { { '0', 'c' }, { '0', '6' }, { '0', 'i' }, { '0', 'f' } },
    { { '0', 'c' }, { '6', 'c' }, { '3', 'i' }, { '3', '3' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
    { { '0', '6' }, { '0', 'i' }, { '3', '3' }, { '3', '3' } } }
};
char letter_G[6][5][2] = {
  { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
  { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } },
  { { '0', 'c' }, { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' } },
  { { '0', 'c' }, { '0', 'c' }, { '0', '6' }, { 'c', 'i' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
  { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } }
};
char letter_O[6][5][2] = {
  { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
  { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
  { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '0', 'i' } }
};
char letter_D[6][5][2] = {
  { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '9', 'i' }, { '3', '3' } },
  { { '0', 'c' }, { '6', 'c' }, { '9', 'i' }, { '9', '9' }, { 'c', 'l' } },
  { { '0', 'c' }, { '0', 'c' }, { 'l', 'l' }, { 'c', 'l' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { 'f', 'f' }, { '0', 'f' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', '6' }, { '3', 'i' }, { '3', '3' }, { '0', 'f' } },
  { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { '3', 'i' }, { '3', '3' } }
};
char letter_N[6][5][2] = {
  { { '6', 'c' }, { 'c', 'i' }, { '3', '3' }, { '6', 'c' }, { 'c', 'i' } },
  { { '0', 'c' }, { '0', '9' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '9', 'c' }, { '9', 'l' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '9', 'l' }, { '0', 'l' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { 'c', 'l' }, { '0', 'c' } },
  { { '0', '6' }, { '0', 'i' }, { '3', '3' }, { '0', '6' }, { '0', 'i' } }
};
char letter_I[6][3][2] = {
  { { '6', 'c' }, { '6', 'i' }, { 'c', 'i' } },
  { { '0', 'c' }, { 'c', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', '0' }, { '0', 'c' } },
  { { '0', '6' }, { '6', 'i' }, { '0', 'i' } }
};
char letter_H[6][5][2] = {
  { { '6', 'c' }, { 'c', 'i' }, { '3', '3' }, { '6', 'c' }, { 'c', 'i' } },
  { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
  { { '0', 'c' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
  { { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' } },
  { { '0', '6' }, { '0', 'i' }, { '3', '3' }, { '0', '6' }, { '0', 'i' } }
};
char letter_T[6][6][2] = {
  { { '6', 'c' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' } },
  { { '0', '6' }, { '6', 'i' }, { 'c', 'i' }, { '6', 'c' }, { '6', 'i' }, { '0', 'i' } },
  { { '3', '3' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
  { { '3', '3' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
  { { '3', '3' }, { '3', '3' }, { '0', 'c' }, { '0', 'c' }, { '3', '3' }, { '3', '3' } },
  { { '3', '3' }, { '3', '3' }, { '0', '6' }, { '0', 'i' }, { '3', '3' }, { '3', '3' } }
};
char base[12][24][2];
char hexNum[24] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' };

void connectToWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  //printWifiStatus();
}

void setup() {
  //Serial.begin(9600);
  //strip.begin();
  //strip.setBrightness(50);
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(WinterTimeOutPin, OUTPUT);
  pinMode(WinterTimeInputPin, INPUT);
  digitalWrite(WinterTimeOutPin, HIGH);
  delay(50);
  if (digitalRead(WinterTimeInputPin) == HIGH) {
    Serial.println("Winter Time");
    winterTime = 1;
  }
  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_pressed, RISING);
  randomSeed(analogRead(randomInputPin));
  Wire.begin();
  connectToWiFi();
  RTC.begin();
  timeClient.begin();
  if (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  updateRTC();
  showRandom();
}
void updateRTC() {
  unsigned long unixTime = timeClient.getEpochTime();
  //Serial.print("Unix time = ");
  //Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime - winterTime * 3600);
  RTC.setTime(timeToSet);
  delay(50);
}
void loop() {
  /////////////////////////////////
  timeClient.update();
  RTC.getTime(currentTime);
  if (RTCMinute < 0 || (currentTime.getMinutes() != RTCMinute && currentTime.getMinutes() % 10 == 0)) {
    updateRTC();
    RTC.getTime(currentTime);
    delay(50);
    RTCMinute = currentTime.getMinutes();
  }
  if (currentTime.getMinutes() != lastMinute) {
    if (patternSelect) {
      patternSelect = false;
      sleepFlag = false;
    } else {
      if (currentTime.getHour() >= 0 && currentTime.getHour() < 9) {
        currentPattern = 15;
      } else {
        sleepFlag = false;
        currentPattern = random(MAX_PATTERN);
      }
    }
    //updateLED(currentPattern);
    //currentPattern=0;
    switch (currentPattern) {
        //switch (6) {
      case 0:
        showRandom();
        break;
      case 1:
        showWave_new();
        break;
      case 2:
        showWave_rectangle();
        break;
      case 3:
        showWave_square_small();
        break;
      case 4:
        showWave_panels();
        break;
      case 5:
        showWave_individual();
        break;
      case 6:
        showWave_square_large();
        break;
      case 7:
        showWave_center();
        break;
      case 8:
        showWave_double_center();
        break;
      case 15:
        if (!sleepFlag) {
          sleepLetter2();
        }
        break;
      default:
        showRandom();
        break;
    }
    if (currentPattern != 15) {
      showDigits();
    }
    RTC.getTime(currentTime);
    lastMinute = currentTime.getMinutes();
  }
  delay(1000);
  /////////////////test code////////////////
  /*if (Serial.available() > 0) {
    String user_input;
    user_input = Serial.readString();
    user_input.trim();
    if (user_input == "test") {
      showDigits();
    } else {
      for (int i = 0; i <= 7; i++) {
        sendData[i] = user_input[i];
      }
      for (int i = 1; i <= 72; i++) {
        Wire.beginTransmission(i);
        Wire.write(sendData);
        Wire.endTransmission();
      }
      Serial.println(sendData);
    }
  }*/
}
void showDigits() {
  RTC.getTime(currentTime);
  //Serial.println("showDigitis");
  int h1 = currentTime.getHour() / 10;
  int h2 = currentTime.getHour() % 10;
  int m1 = currentTime.getMinutes() / 10;
  int m2 = currentTime.getMinutes() % 10;

  for (int i = 1; i <= 10; i++) {
    for (int j = 1; j <= 22; j++) {
      base[i][j][0] = '3';
      base[i][j][1] = '3';
    }
  }
  for (int i = 1; i <= 10; i++) {
    base[i][0][0] = '0';
    base[i][0][1] = 'c';
    base[i][23][0] = '0';
    base[i][23][1] = 'c';
  }
  for (int i = 1; i <= 22; i++) {
    base[0][i][0] = '6';
    base[0][i][1] = 'i';
    base[11][i][0] = '6';
    base[11][i][1] = 'i';
  }
  base[0][0][0] = '6';
  base[0][0][1] = 'c';
  base[0][23][0] = 'c';
  base[0][23][1] = 'i';
  base[11][0][0] = '0';
  base[11][0][1] = '6';
  base[11][23][0] = '0';
  base[11][23][1] = 'i';

  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 4; j++) {
      base[i + 3][j + 1][0] = digits[h1][i][j][0];
      base[i + 3][j + 1][1] = digits[h1][i][j][1];

      base[i + 3][j + 6][0] = digits[h2][i][j][0];
      base[i + 3][j + 6][1] = digits[h2][i][j][1];

      base[i + 3][j + 13][0] = digits[m1][i][j][0];
      base[i + 3][j + 13][1] = digits[m1][i][j][1];

      base[i + 3][j + 18][0] = digits[m2][i][j][0];
      base[i + 3][j + 18][1] = digits[m2][i][j][1];
    }
  }
  base[4][11][0] = '6';
  base[4][11][1] = 'c';
  base[4][12][0] = 'c';
  base[4][12][1] = 'i';
  base[5][11][0] = '0';
  base[5][11][1] = '6';
  base[5][12][0] = '0';
  base[5][12][1] = 'i';

  base[6][11][0] = '6';
  base[6][11][1] = 'c';
  base[6][12][0] = 'c';
  base[6][12][1] = 'i';
  base[7][11][0] = '0';
  base[7][11][1] = '6';
  base[7][12][0] = '0';
  base[7][12][1] = 'i';

  for (int i = 1; i <= 72; i++) {
    int blocks = (i - 1) / 9;
    int blocksInner = (i - 1) % 9;
    int blockRow = blocks / 4;
    int blockColumn = blocks % 4;
    int blockInnerRow = blocksInner / 3;
    int blockInnerColumn = blocksInner % 3;
    int realRow = (blockRow * 3 + blockInnerRow) * 2;
    int realColumn = (blockColumn * 3 + blockInnerColumn) * 2;
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
}
void sleepLetter() {
  for (int i = 0; i <= 11; i++) {
    for (int j = 0; j <= 23; j++) {
      base[i][j][0] = '3';
      base[i][j][1] = '3';
    }
  }
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 3; j++) {
      for (int k = 0; k <= 1; k++) {
        base[i + 3][j + 0][k] = letters[0][i][j][k];
        base[i + 3][j + 5][k] = letters[1][i][j][k];
        base[i + 3][j + 10][k] = letters[2][i][j][k];
        base[i + 3][j + 15][k] = letters[2][i][j][k];
        base[i + 3][j + 20][k] = letters[3][i][j][k];
      }
    }
  }
  for (int i = 1; i <= 72; i++) {
    int blocks = (i - 1) / 9;
    int blocksInner = (i - 1) % 9;
    int blockRow = blocks / 4;
    int blockColumn = blocks % 4;
    int blockInnerRow = blocksInner / 3;
    int blockInnerColumn = blocksInner % 3;
    int realRow = (blockRow * 3 + blockInnerRow) * 2;
    int realColumn = (blockColumn * 3 + blockInnerColumn) * 2;
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  sleepFlag = true;
}
void sleepLetter2() {
  for (int i = 0; i <= 5; i++) {
    for (int j = 20; j <= 23; j++) {
      base[i][j][0] = '3';
      base[i][j][1] = '3';
    }
  }
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 4; j++) {
      for (int k = 0; k <= 1; k++) {
        base[i][j + 0][k] = letter_G[i][j][k];
        base[i][j + 5][k] = letter_O[i][j][k];
        base[i][j + 10][k] = letter_O[i][j][k];
        base[i][j + 15][k] = letter_D[i][j][k];
        base[i + 6][j][k] = letter_N[i][j][k];
        base[i + 6][j + 8][k] = letter_G[i][j][k];
        base[i + 6][j + 13][k] = letter_H[i][j][k];
      }
    }
  }
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 2; j++) {
      for (int k = 0; k <= 1; k++) {
        base[i + 6][j + 5][k] = letter_I[i][j][k];
      }
    }
  }
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 5; j++) {
      for (int k = 0; k <= 1; k++) {
        base[i + 6][j + 18][k] = letter_T[i][j][k];
      }
    }
  }
  for (int i = 1; i <= 72; i++) {
    int blocks = (i - 1) / 9;
    int blocksInner = (i - 1) % 9;
    int blockRow = blocks / 4;
    int blockColumn = blocks % 4;
    int blockInnerRow = blocksInner / 3;
    int blockInnerColumn = blocksInner % 3;
    int realRow = (blockRow * 3 + blockInnerRow) * 2;
    int realColumn = (blockColumn * 3 + blockInnerColumn) * 2;
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  sleepFlag = true;
}
void showRandom() {
  setDirection(random(2));
  for (int i = 0; i <= 11; i++) {
    for (int j = 0; j <= 23; j++) {
      base[i][j][0] = hexNum[random(24)];
      base[i][j][1] = hexNum[random(24)];
    }
  }
  for (int i = 1; i <= 72; i++) {
    for (int j = 0; j <= 3; j++) {
      int row = (i - 1) / 6;
      int coloum = (i - 1) % 6;
      sendData[j * 2] = base[row][coloum * 4 + j][0];
      sendData[j * 2 + 1] = base[row][coloum * 4 + j][1];
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  readyDelay();
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("CCCCCCCC");
    Wire.endTransmission();
    delay(2);
  }
}
void showWave_new() {
  //Serial.println("showWave_new");
  //直线波浪
  int transformMap[6][12] = { { 1, 2, 3, 10, 11, 12, 19, 20, 21, 28, 29, 30 },
                              { 4, 5, 6, 13, 14, 15, 22, 23, 24, 31, 32, 33 },
                              { 7, 8, 9, 16, 17, 18, 25, 26, 27, 34, 35, 36 },
                              { 37, 38, 39, 46, 47, 48, 55, 56, 57, 64, 65, 66 },
                              { 40, 41, 42, 49, 50, 51, 58, 59, 60, 67, 68, 69 },
                              { 43, 44, 45, 52, 53, 54, 61, 62, 63, 70, 71, 72 } };
  setDirection(random(2));
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("3f3f3f3f");
    Wire.endTransmission();
    delay(2);
  }
  readyDelay();
  delay(2000);
  int row, column, innerRow, innerColumn, innerN, waveFlag;
  float centerX = random(2) * 11;
  float centerY = random(2) * 23;
  for (int total = 0; total <= sqrt(23 * 23 + 11 * 11) + 1; total++) {
    waveFlag = 0;
    for (int i = 0; i <= 11; i++) {
      for (int j = 0; j <= 23; j++) {
        float dis = sqrt((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY));
        if (dis > (total - 1) && dis <= total) {
          row = i / 2;
          innerRow = i % 2;
          column = j / 2;
          innerColumn = j % 2;
          innerN = innerRow * 2 + innerColumn;
          for (int k = 0; k <= 7; k++) {
            sendData[k] = '/';
          }
          sendData[innerN * 2] = 'D';
          sendData[innerN * 2 + 1] = 'D';
          Wire.beginTransmission(transformMap[row][column]);
          Wire.write(sendData, 8);
          Wire.endTransmission();
          delay(2);
          //delay(10);
          waveFlag = 1;
        }
      }
    }
    delay(250 * waveFlag);
  }
}
void showWave_center() {
  //Serial.println("showWave_center");
  //中心波浪
  int transformMap[6][12] = { { 1, 2, 3, 10, 11, 12, 19, 20, 21, 28, 29, 30 },
                              { 4, 5, 6, 13, 14, 15, 22, 23, 24, 31, 32, 33 },
                              { 7, 8, 9, 16, 17, 18, 25, 26, 27, 34, 35, 36 },
                              { 37, 38, 39, 46, 47, 48, 55, 56, 57, 64, 65, 66 },
                              { 40, 41, 42, 49, 50, 51, 58, 59, 60, 67, 68, 69 },
                              { 43, 44, 45, 52, 53, 54, 61, 62, 63, 70, 71, 72 } };
  float centerX = 11 / 2.0;
  float centerY = 23 / 2.0;
  float Pi = 3.1415926;
  int flip = random(2);
  if (flip == 0) {
    flip = -1;
  }
  int row, column, innerRow, innerColumn, innerN, waveFlag;
  char p;
  setDirection(1);
  for (int i = 0; i <= 11; i++) {
    for (int j = 0; j <= 23; j++) {
      float theta = atan2(flip * (centerY - j), flip * (i - centerX));
      if (theta < 0) {
        theta = theta + 2.0 * Pi;
      }
      int roundTheta = round(theta * 12 / Pi);
      if (roundTheta > 23) {
        p = '0';
      } else if (roundTheta > 9) {
        p = 'a' + (roundTheta - 10);
      } else {
        p = '0' + roundTheta;
      }
      row = i / 2;
      innerRow = i % 2;
      column = j / 2;
      innerColumn = j % 2;
      innerN = innerRow * 2 + innerColumn;
      for (int k = 0; k <= 7; k++) {
        sendData[k] = '/';
      }
      sendData[innerN * 2] = p;
      sendData[innerN * 2 + 1] = p;
      Wire.beginTransmission(transformMap[row][column]);
      Wire.write(sendData, 8);
      Wire.endTransmission();
      delay(2);
    }
  }
  readyDelay();
  delay(2000);

  for (int total = 0; total <= sqrt(23 * 23 + 11 * 11) + 1; total++) {
    waveFlag = 0;
    for (int i = 0; i <= 11; i++) {
      for (int j = 0; j <= 23; j++) {
        float dis = sqrt((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY));
        if (dis > (total - 1) && dis <= total) {
          row = i / 2;
          innerRow = i % 2;
          column = j / 2;
          innerColumn = j % 2;
          innerN = innerRow * 2 + innerColumn;
          for (int k = 0; k <= 7; k++) {
            sendData[k] = '/';
          }
          sendData[innerN * 2] = 'D';
          sendData[innerN * 2 + 1] = 'D';
          Wire.beginTransmission(transformMap[row][column]);
          Wire.write(sendData, 8);
          Wire.endTransmission();
          delay(2);
          waveFlag = 1;
        }
      }
    }
    delay(400 * waveFlag);
  }
}
void showWave_double_center() {
  //Serial.println("showWave_center");
  //中心波浪
  int transformMap[6][12] = { { 1, 2, 3, 10, 11, 12, 19, 20, 21, 28, 29, 30 },
                              { 4, 5, 6, 13, 14, 15, 22, 23, 24, 31, 32, 33 },
                              { 7, 8, 9, 16, 17, 18, 25, 26, 27, 34, 35, 36 },
                              { 37, 38, 39, 46, 47, 48, 55, 56, 57, 64, 65, 66 },
                              { 40, 41, 42, 49, 50, 51, 58, 59, 60, 67, 68, 69 },
                              { 43, 44, 45, 52, 53, 54, 61, 62, 63, 70, 71, 72 } };
  float centerX = 11 / 2.0;
  float centerY = 11 / 2.0;
  float Pi = 3.1415926;
  int flip = random(2);
  if (flip == 0) {
    flip = -1;
  }
  int row, column, innerRow, innerColumn, innerN, waveFlag;
  char p;
  setDirection(1);
  for (int i = 0; i <= 11; i++) {
    for (int j = 0; j <= 11; j++) {
      float theta = atan2(flip * (centerY - j), flip * (i - centerX));
      if (theta < 0) {
        theta = theta + 2.0 * Pi;
      }
      int roundTheta = round(theta * 12 / Pi);
      if (roundTheta > 23) {
        p = '0';
      } else if (roundTheta > 9) {
        p = 'a' + (roundTheta - 10);
      } else {
        p = '0' + roundTheta;
      }
      row = i / 2;
      innerRow = i % 2;
      column = j / 2;
      innerColumn = j % 2;
      innerN = innerRow * 2 + innerColumn;
      for (int k = 0; k <= 7; k++) {
        sendData[k] = '/';
      }
      sendData[innerN * 2] = p;
      sendData[innerN * 2 + 1] = p;
      Wire.beginTransmission(transformMap[row][column]);
      Wire.write(sendData, 8);
      Wire.endTransmission();
      delay(2);
      Wire.beginTransmission(transformMap[row][column + 6]);
      Wire.write(sendData, 8);
      Wire.endTransmission();
      delay(2);
    }
  }
  readyDelay();
  delay(2000);

  for (int total = 0; total <= sqrt(11 * 11 + 11 * 11) + 1; total++) {
    waveFlag = 0;
    for (int i = 0; i <= 11; i++) {
      for (int j = 0; j <= 11; j++) {
        float dis = sqrt((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY));
        if (dis > (total - 1) && dis <= total) {
          row = i / 2;
          innerRow = i % 2;
          column = j / 2;
          innerColumn = j % 2;
          innerN = innerRow * 2 + innerColumn;
          for (int k = 0; k <= 7; k++) {
            sendData[k] = '/';
          }
          sendData[innerN * 2] = 'D';
          sendData[innerN * 2 + 1] = 'D';
          Wire.beginTransmission(transformMap[row][column]);
          Wire.write(sendData, 8);
          Wire.endTransmission();
          delay(2);
          Wire.beginTransmission(transformMap[row][column + 6]);
          Wire.write(sendData, 8);
          Wire.endTransmission();
          delay(2);
          waveFlag = 1;
        }
      }
    }
    delay(400 * waveFlag);
  }
}
void showWave_rectangle() {
  //大长方形
  //Serial.println("showWave2");
  setDirection(1);
  for (int i = 0; i <= 5; i++) {
    base[i][i][0] = '6';
    base[i][i][1] = 'c';
    base[11 - i][i][0] = '0';
    base[11 - i][i][1] = '6';
    base[i][23 - i][0] = 'c';
    base[i][23 - i][1] = 'i';
    base[11 - i][23 - i][0] = 'i';
    base[11 - i][23 - i][1] = '0';
    for (int j = i + 1; j <= 23 - i - 1; j++) {
      base[i][j][0] = '6';
      base[i][j][1] = 'i';
      base[11 - i][j][0] = 'i';
      base[11 - i][j][1] = '6';
    }
    for (int j = i + 1; j <= 11 - i - 1; j++) {
      base[j][i][0] = '0';
      base[j][i][1] = 'c';
      base[j][23 - i][0] = 'c';
      base[j][23 - i][1] = '0';
    }
  }
  for (int i = 1; i <= 72; i++) {
    int blocks = (i - 1) / 9;
    int blocksInner = (i - 1) % 9;
    int blockRow = blocks / 4;
    int blockColumn = blocks % 4;
    int blockInnerRow = blocksInner / 3;
    int blockInnerColumn = blocksInner % 3;
    int realRow = (blockRow * 3 + blockInnerRow) * 2;
    int realColumn = (blockColumn * 3 + blockInnerColumn) * 2;
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  readyDelay();
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("DDDDDDDD");
    Wire.endTransmission();
    delay(2);
  }
}
void showWave_square_small() {
  //小正方形
  setDirection(1);
  for (int k = 0; k <= 1; k++) {
    for (int l = 0; l <= 3; l++)
      for (int i = 0; i <= 2; i++) {
        base[i + k * 6][i + l * 6][0] = '6';
        base[i + k * 6][i + l * 6][1] = 'c';
        base[5 - i + k * 6][i + l * 6][0] = '0';
        base[5 - i + k * 6][i + l * 6][1] = '6';
        base[i + k * 6][5 - i + l * 6][0] = 'c';
        base[i + k * 6][5 - i + l * 6][1] = 'i';
        base[5 - i + k * 6][5 - i + l * 6][0] = 'i';
        base[5 - i + k * 6][5 - i + l * 6][1] = '0';
        for (int j = i + 1; j <= 5 - i - 1; j++) {
          base[i + k * 6][j + l * 6][0] = '6';
          base[i + k * 6][j + l * 6][1] = 'i';
          base[5 - i + k * 6][j + l * 6][0] = 'i';
          base[5 - i + k * 6][j + l * 6][1] = '6';
        }
        for (int j = i + 1; j <= 5 - i - 1; j++) {
          base[j + k * 6][i + l * 6][0] = '0';
          base[j + k * 6][i + l * 6][1] = 'c';
          base[j + k * 6][5 - i + l * 6][0] = 'c';
          base[j + k * 6][5 - i + l * 6][1] = '0';
        }
      }
  }
  for (int i = 1; i <= 72; i++) {
    int blocks = (i - 1) / 9;
    int blocksInner = (i - 1) % 9;
    int blockRow = blocks / 4;
    int blockColumn = blocks % 4;
    int blockInnerRow = blocksInner / 3;
    int blockInnerColumn = blocksInner % 3;
    int realRow = (blockRow * 3 + blockInnerRow) * 2;
    int realColumn = (blockColumn * 3 + blockInnerColumn) * 2;
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  readyDelay();
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("DDDDDDDD");
    Wire.endTransmission();
    delay(2);
  }
}
void showWave_panels() {
  //Serial.println("showWave4!");
  //独立面板
  setDirection(1);
  for (int i = 1; i <= 9; i++) {
    for (int j = 0; j <= 1; j++) {
      Wire.beginTransmission(i + j * 18);
      Wire.write("3f3f3f3f");
      Wire.endTransmission();
      delay(2);
      Wire.beginTransmission(i + 9 + j * 18);
      Wire.write("9l9l9l9l");
      Wire.endTransmission();
      delay(2);
      Wire.beginTransmission(i + 36 + j * 18);
      Wire.write("l9l9l9l9");
      Wire.endTransmission();
      delay(2);
      Wire.beginTransmission(i + 45 + j * 18);
      Wire.write("f3f3f3f3");
      Wire.endTransmission();
      delay(2);
    }
  }
  readyDelay();
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("CCCCCCCC");
    Wire.endTransmission();
    delay(2);
  }
}
void showWave_individual() {
  setDirection(random(2));
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("6c06cii0");
    Wire.endTransmission();
    delay(2);
  }
  readyDelay();
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("CCCCCCCC");
    Wire.endTransmission();
    delay(2);
  }
}
void showWave_square_large() {
  //大正方形
  setDirection(1);
  for (int i = 0; i <= 5; i++) {
    base[5 - i][11 - i][0] = '6';
    base[5 - i][11 - i][1] = 'c';
    base[11 - 5 + i][11 - i][0] = '0';
    base[11 - 5 + i][11 - i][1] = '6';
    base[5 - i][23 - 11 + i][0] = 'c';
    base[5 - i][23 - 11 + i][1] = 'i';
    base[11 - 5 + i][23 - 11 + i][0] = 'i';
    base[11 - 5 + i][23 - 11 + i][1] = '0';
    for (int j = 11 - i + 1; j <= 23 - 11 + i - 1; j++) {
      base[5 - i][j][0] = '6';
      base[5 - i][j][1] = 'i';
      base[11 - 5 + i][j][0] = 'i';
      base[11 - 5 + i][j][1] = '6';
    }
    for (int j = 5 - i + 1; j <= 11 - 5 + i - 1; j++) {
      base[j][11 - i][0] = '0';
      base[j][11 - i][1] = 'c';
      base[j][23 - 11 + i][0] = 'c';
      base[j][23 - 11 + i][1] = '0';
    }
  }
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 5; j++) {
      base[i][j][0] = '0';
      base[i][j][1] = 'c';
      base[11 - i][j][0] = '0';
      base[11 - i][j][1] = 'c';
      base[i][23 - j][0] = 'c';
      base[i][23 - j][1] = '0';
      base[11 - i][23 - j][0] = 'c';
      base[11 - i][23 - j][1] = '0';
      /*base[i][j][0] = 'i';
      base[i][j][1] = '0';
      base[11 - i][j][0] = 'c';
      base[11 - i][j][1] = 'i';
      base[i][23 - j][0] = '0';
      base[i][23 - j][1] = '6';
      base[11 - i][23 - j][0] = '6';
      base[11 - i][23 - j][1] = 'c';*/
    }
  }
  for (int i = 1; i <= 72; i++) {
    int blocks = (i - 1) / 9;
    int blocksInner = (i - 1) % 9;
    int blockRow = blocks / 4;
    int blockColumn = blocks % 4;
    int blockInnerRow = blocksInner / 3;
    int blockInnerColumn = blocksInner % 3;
    int realRow = (blockRow * 3 + blockInnerRow) * 2;
    int realColumn = (blockColumn * 3 + blockInnerColumn) * 2;
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  readyDelay();
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("DDDDDDDD");
    Wire.endTransmission();
    delay(2);
  }
}
void goZero() {
  //Serial.println("goZero");
  for (int i = 0; i <= 7; i++) {
    sendData[i] = '0';
  }
  for (int i = 1; i <= 72; i++) {
    //Serial.println(i);
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    delay(2);
  }
  delay(10000);
}
void reZero() {
  //Serial.println("reZero");
  sendData[0] = 'v';
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write(sendData, 1);
    Wire.endTransmission();
    delay(2);
  }
  delay(20000);
}
void setDirection(int dType) {
  //Serial.print("setDirection-");
  //Serial.println(dType);
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (statusTemp == false) {
      Wire.requestFrom(i, 1);
      if (Wire.read()) {
        statusTemp = true;
      } else {
        delay(500);
      }
    }
    Wire.beginTransmission(i);
    if (dType == 0) {
      Wire.write("x", 1);
    } else if (dType == 1) {
      Wire.write("z", 1);
    }
    Wire.endTransmission();
    delay(2);
  }
  if (preDirection != dType) {
    for (int i = 1; i <= 72; i++) {
      Wire.beginTransmission(i);
      Wire.write("BABABABA", 8);
      Wire.endTransmission();
      delay(2);
    }
    preDirection = dType;
  }
}
void readyDelay() {
  unsigned long readyMillis = millis();
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (statusTemp == false && (millis() - readyMillis) < 45000) {
      Wire.requestFrom(i, 1);
      while (Wire.available()) {
        statusTemp = Wire.read();
      }
      if (statusTemp == false) {
        delay(500);
      } else {
        //Serial.print(i);
        //Serial.println(":ready!");
      }
    }
    delay(2);
  }
  //Serial.println("readyDelay finished!");
}

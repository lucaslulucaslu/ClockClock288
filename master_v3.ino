#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>

RTC_DS3231 rtc;
int lastMinute = 0;
int winterTime=0;
int preDirection=0;
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
  { { { '3', 'f' }, { '6', 'c' }, { '6', 'i' }, { 'c', 'i' }, { '3', 'f' } },
    { { '3', 'f' }, { '0', 'c' }, { 'c', 'c' }, { '0', 'c' }, { '3', 'f' } },
    { { '3', 'f' }, { '0', 'c' }, { '0', 'c' }, { '0', 'c' }, { '3', 'f' } },
    { { '3', 'f' }, { '0', 'c' }, { '0', 'c' }, { '0', 'c' }, { '3', 'f' } },
    { { '3', 'f' }, { '0', 'c' }, { '0', '0' }, { '0', 'c' }, { '3', 'f' } },
    { { '3', 'f' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '3', 'f' } } },
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
  { { { '6', 'c' }, { 'c', 'i' }, { '3', 'f' }, { '6', 'c' }, { 'c', 'i' } },
    { { '0', 'c' }, { '0', 'c' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' } },
    { { '0', 'c' }, { '0', '6' }, { '6', 'i' }, { '0', 'i' }, { '0', 'c' } },
    { { '0', '6' }, { '6', 'i' }, { '6', 'i' }, { 'c', 'i' }, { '0', 'c' } },
    { { '3', 'f' }, { '3', 'f' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' } },
    { { '3', 'f' }, { '3', 'f' }, { '3', 'f' }, { '0', '6' }, { '0', 'i' } } },
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
    { { '3', 'f' }, { '3', 'f' }, { '3', 'f' }, { '0', 'f' }, { '0', 'f' } },
    { { '3', 'f' }, { '3', 'f' }, { '3', 'c' }, { '3', 'c' }, { '3', 'f' } },
    { { '3', 'f' }, { '3', 'f' }, { '0', 'c' }, { '0', 'c' }, { '3', 'f' } },
    { { '3', 'f' }, { '3', 'f' }, { '0', '6' }, { '0', 'i' }, { '3', 'f' } } },
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
char base[12][24][2];
char hexNum[24] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' };
uint32_t syncProvider() {
  return rtc.now().unixtime()-winterTime*3600;
}
void setup() {
  pinMode(2,OUTPUT);
  pinMode(3,INPUT);
  digitalWrite(2,HIGH);
  delay(10);
  if(digitalRead(3)==HIGH){
    //Serial.println("Winter Time");
    winterTime=1;
  }
  delay(1000);
  Wire.begin();
  Serial.begin(9600);
  rtc.begin();
  setSyncProvider(syncProvider);
  /*if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }*/
  lastMinute = minute();
  //showRandom();

}

void loop() {
  //
  //Serial.println("test");
  //delay(5000);
  if (Serial.available() > 0) {
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
  }
  /////////////////////////////////

  /*int minuteNum = minute();
  if (minuteNum != lastMinute) {
    lastMinute = minuteNum;
    switch (random(7)) {
    //switch (6) {
      case 0:
        showRandom();
        break;
      case 1:
        showWave();
        break;
      case 2:
        showWave2();
        break;
      case 3:
        showWave3();
        break;
      case 4:
        showWave4();
        break;
      case 5:
        showWave5();
        break;
      case 6:
        showWave6();
        break;
      default:
        showRandom();
        break;
    }
    showDigits();
  }*/
  delay(1000);
}

void showDigits() {
  //Serial.println("showDigitis");
  int h1 = hour() / 10;
  int h2 = hour() % 10;
  int m1 = minute() / 10;
  int m2 = minute() % 10;

  for (int i = 1; i <= 10; i++) {
    for (int j = 1; j <= 22; j++) {
      base[i][j][0] = '3';
      base[i][j][1] = 'f';
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
    /*for (int j = 0; j <= 3; j++) {
      int row = (i - 1) / 6;
      int coloum = (i - 1) % 6;
      sendData[j * 2] = base[row][coloum * 4 + j][0];
      sendData[j * 2 + 1] = base[row][coloum * 4 + j][1];
    }*/
    for (int j = 0; j <= 1; j++) {
      for (int k = 0; k <= 1; k++) {
        sendData[(j * 2 + k) * 2] = base[realRow + j][realColumn + k][0];
        sendData[(j * 2 + k) * 2 + 1] = base[realRow + j][realColumn + k][1];
      }
    }
    Wire.beginTransmission(i);
    Wire.write(sendData, 8);
    Wire.endTransmission();
    //delay(10);
  }
}
void showRandom() {
  bool readyStatus[72];
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
    readyStatus[i - 1] = false;
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  delay(3000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("CCCCCCCC");
    Wire.endTransmission();
  }
}
void showWave() {
  //小直线波浪
  int transformMap[6][12] = { { 1, 2, 3, 10, 11, 12, 19, 20, 21, 28, 29, 30 },
                              { 4, 5, 6, 13, 14, 15, 22, 23, 24, 31, 32, 33 },
                              { 7, 8, 9, 16, 17, 18, 25, 26, 27, 34, 35, 36 },
                              { 37, 38, 39, 46, 47, 48, 55, 56, 57, 64, 65, 66 },
                              { 40, 41, 42, 49, 50, 51, 58, 59, 60, 67, 68, 69 },
                              { 43, 44, 45, 52, 53, 54, 61, 62, 63, 70, 71, 72 } };
  bool readyStatus[72];
  int startCenter = random(2);
  int tempNumber;

  setDirection(random(2));
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("3f3f3f3f");
    Wire.endTransmission();
    readyStatus[i - 1] = false;
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  if (startCenter == 1) {
    int startPointX = random(2);
    int startPointY = random(2);
    if (startPointX == 1) {
      for (int i = 0; i <= 2; i++) {
        for (int j = 0; j <= 11; j++) {
          tempNumber = transformMap[i][j];
          transformMap[i][j] = transformMap[5 - i][j];
          transformMap[5 - i][j] = tempNumber;
        }
      }
    }
    if (startPointY == 1) {
      for (int i = 0; i <= 5; i++) {
        for (int j = 0; j <= 5; j++) {
          tempNumber = transformMap[i][j];
          transformMap[i][j] = transformMap[i][11 - j];
          transformMap[i][11 - j] = tempNumber;
        }
      }
    }
    for (int i = 0; i <= 11; i++) {
      for (int j = 0; j < i; j++) {
        if (i <= 5) {
          Wire.beginTransmission(transformMap[i][j]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          delay(100);
        }
        if (j <= 5) {
          Wire.beginTransmission(transformMap[j][i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          delay(100);
        }
      }
      if (i <= 5) {
        Wire.beginTransmission(transformMap[i][i]);
        Wire.write("CCCCCCCC");
        Wire.endTransmission();
        delay(100);
      }
    }
  } else {
    for (int i = 0; i <= 5; i++) {
      if (i <= 2) {
        for (int j = 0; j < i; j++) {
          Wire.beginTransmission(transformMap[2 - i][5 - j]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[2 - i][11 - 5 + j]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[5 - 2 + i][5 - j]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[5 - 2 + i][11 - 5 + j]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();

          Wire.beginTransmission(transformMap[2 - j][5 - i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[2 - j][11 - 5 + i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[5 - 2 + j][5 - i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[5 - 2 + j][11 - 5 + i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          delay(1000);
        }
        Wire.beginTransmission(transformMap[2 - i][5 - i]);
        Wire.write("CCCCCCCC");
        Wire.endTransmission();
        Wire.beginTransmission(transformMap[2 - i][11 - 5 + i]);
        Wire.write("CCCCCCCC");
        Wire.endTransmission();
        Wire.beginTransmission(transformMap[5 - 2 + i][5 - i]);
        Wire.write("CCCCCCCC");
        Wire.endTransmission();
        Wire.beginTransmission(transformMap[5 - 2 + i][11 - 5 + i]);
        Wire.write("CCCCCCCC");
        Wire.endTransmission();
        delay(1000);
      } else {
        for (int j = 0; j <= 2; j++) {
          Wire.beginTransmission(transformMap[j][5 - i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[j][11 - 5 + i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[5 - j][5 - i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
          Wire.beginTransmission(transformMap[5 - j][11 - 5 + i]);
          Wire.write("CCCCCCCC");
          Wire.endTransmission();
        }
        delay(1000);
      }
    }
  }
}
void showWave2() {
  //大长方形
  //Serial.println("showWave2");
  bool readyStatus[72];
  setDirection(random(2));
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
    //delay(10);
    readyStatus[i - 1] = false;
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("DDDDDDDD");
    Wire.endTransmission();
  }
}
void showWave3() {
  //小正方形
  bool readyStatus[72];
  setDirection(random(2));
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
    readyStatus[i - 1] = false;
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("DDDDDDDD");
    Wire.endTransmission();
  }
}
void showWave4() {
  //Serial.println("showWave4!");
  //独立面板
  setDirection(random(2));
  bool readyStatus[72];
  for (int i = 1; i <= 9; i++) {
    for (int j = 0; j <= 1; j++) {
      Wire.beginTransmission(i + j * 18);
      Wire.write("3f3f3f3f");
      Wire.endTransmission();
      Wire.beginTransmission(i + 9 + j * 18);
      Wire.write("9l9l9l9l");
      Wire.endTransmission();
      Wire.beginTransmission(i + 36 + j * 18);
      Wire.write("l9l9l9l9");
      Wire.endTransmission();
      Wire.beginTransmission(i + 45 + j * 18);
      Wire.write("f3f3f3f3");
      Wire.endTransmission();
      readyStatus[i - 1] = false;
    }
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("CCCCCCCC");
    Wire.endTransmission();
  }
}
void showWave5() {
  //独立反对角正方形
  setDirection(random(2));
  bool readyStatus[72];
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("6c06cii0");
    Wire.endTransmission();
    readyStatus[i - 1] = false;
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("CCCCCCCC");
    Wire.endTransmission();
  }
}
void showWave6() {
  //大正方形
  bool readyStatus[72];
  setDirection(random(2));
  for (int i = 0; i <= 5; i++) {
    base[5-i][11-i][0] = '6';
    base[5-i][11-i][1] = 'c';
    base[11 -5+ i][11-i][0] = '0';
    base[11 -5 +i][11-i][1] = '6';
    base[5-i][23-11 + i][0] = 'c';
    base[5-i][23-11 + i][1] = 'i';
    base[11-5 + i][23-11 + i][0] = 'i';
    base[11 -5+ i][23 -11+ i][1] = '0';
    for (int j = 11-i + 1; j <= 23- 11+ i - 1; j++) {
      base[5-i][j][0] = '6';
      base[5-i][j][1] = 'i';
      base[11-5 + i][j][0] = 'i';
      base[11 -5+ i][j][1] = '6';
    }
    for (int j = 5-i + 1; j <= 11 -5+ i - 1; j++) {
      base[j][11-i][0] = '0';
      base[j][11-i][1] = 'c';
      base[j][23-11 + i][0] = 'c';
      base[j][23-11 + i][1] = '0';
    }
  }
  for(int i=0;i<=5;i++){
    for(int j=0;j<=5;j++){
      base[i][j][0]='i';
      base[i][j][1]='0';
      base[11-i][j][0]='c';
      base[11-i][j][1]='i';
      base[i][23-j][0]='0';
      base[i][23-j][1]='6';
      base[11-i][23-j][0]='6';
      base[11-i][23-j][1]='c';
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
    readyStatus[i - 1] = false;
  }
  for (int i = 1; i <= 72; i++) {
    bool statusTemp = false;
    while (readyStatus[i - 1] == false) {
      Wire.requestFrom(i, 1);
      statusTemp = Wire.read();
      if (statusTemp) {
        readyStatus[i - 1] = true;
      } else {
        delay(1000);
      }
    }
  }
  delay(2000);
  for (int i = 1; i <= 72; i++) {
    Wire.beginTransmission(i);
    Wire.write("DDDDDDDD");
    Wire.endTransmission();
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
    delay(10);
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
    delay(10);
  }
  delay(20000);
}
void setDirection(int dType) {
  //Serial.println("setDirection!");
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
      Wire.write("x");
    } else if (dType == 1) {
      Wire.write("z");
    }
    Wire.endTransmission();
  }
  if(preDirection!=dType){
      for (int i = 1; i <= 72; i++) {
        Wire.beginTransmission(i);
        Wire.write("BABABABA");
        Wire.endTransmission();
      }
      preDirection=dType;
    }
}

#include <Wire.h>
const int DIR_A = 0;
const int DIR_B = 1;
const int RESET = 13;
const int delta_A = 48;
const int delta_B = 36;
unsigned long previousMills = 0;
const long DELAY = 2;
const int STEPS = 360 * 12;
int position_current[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int position_input[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int position_diff[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool forward1 = false;
bool forward2 = false;
bool zeroFlag = false;
int turns_input[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int hall_sensors[] = { 10, A1, 11, A2, 12, A6, A0, A7 };
int hall_flags[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int hall_positions[8][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
const int hall_delta[8] = { 48, 36, 48, 36, 48, 36, 48, 36 };
int STEP[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
void setup() {
  int tempVal;
  for (int i = 0; i <= 7; i = i + 2) {
    pinMode(STEP[i], OUTPUT);
    digitalWrite(STEP[i], LOW);
    pinMode(hall_sensors[i], INPUT);
    tempVal = digitalRead(hall_sensors[i]);
    if (tempVal == LOW) {
      hall_flags[i] = 1;
    }
  }
  for (int i = 1; i <= 7; i = i + 2) {
    pinMode(STEP[i], OUTPUT);
    digitalWrite(STEP[i], LOW);
    pinMode(hall_sensors[i], INPUT);
    tempVal = analogRead(hall_sensors[i]);
    if (tempVal < 500) {
      hall_flags[i] = 1;
    }
  }
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(1);  // keep reset low min 1ms
  digitalWrite(RESET, HIGH);
  digitalWrite(DIR_A, forward1 ? HIGH : LOW);
  digitalWrite(DIR_B, forward2 ? HIGH : LOW);
  Wire.begin(72);
  Wire.onReceive(DataReceive);
  Wire.onRequest(statusRequest);
  //Serial.begin(9600);
}
void loop() {
  int tempVal;
  unsigned long currentMills = millis();
  if (currentMills - previousMills >= DELAY && !zeroFlag) {
    previousMills = currentMills;
    for (int i = 0; i <= 7; i = i + 2) {
      if (position_input[i] != position_current[i] || turns_input[i] != 0) {
        digitalWrite(STEP[i], LOW);
        delayMicroseconds(1);
        digitalWrite(STEP[i], HIGH);
        if (forward1) {
          position_current[i]--;
        } else {
          position_current[i]++;
        }
        if (position_current[i] >= STEPS) {
          position_current[i] -= STEPS;
          if (turns_input[i] > 0) {
            turns_input[i]--;
          }
        } else if (position_current[i] <= -1) {
          position_current[i] += STEPS;
          if (turns_input[i] > 0) {
            turns_input[i]--;
          }
        }
      }
      if (digitalRead(hall_sensors[i]) == LOW) {
        tempVal = 1;
      } else {
        tempVal = 0;
      }
      if (tempVal != hall_flags[i]) {
        hall_flags[i] = tempVal;
        hall_positions[i][tempVal] = position_current[i];
        if (tempVal == 0) {
          if ((hall_positions[i][0] < hall_positions[i][1]) != forward1) {
            position_diff[i] = STEPS / 2 - (hall_positions[i][0] + hall_positions[i][1]) / 2;
          } else {
            position_diff[i] = STEPS - (hall_positions[i][0] + hall_positions[i][1]) / 2;
          }
          if (forward1) {
            position_diff[i] -= hall_delta[i];
          } else {
            position_diff[i] += hall_delta[i];
          }
          if (position_diff[i] < 0) {
            position_diff[i] += STEPS;
          }
          position_diff[i] %= STEPS;
          if (position_diff[i] > 24 && position_diff[i] < STEPS - 24) {
            position_current[i] = (position_current[i] + position_diff[i]) % STEPS;
            hall_positions[i][0] = (hall_positions[i][0] + position_diff[i]) % STEPS;
            hall_positions[i][1] = (hall_positions[i][1] + position_diff[i]) % STEPS;
            /*if (i == 0) {
              Serial.print("i:");
              Serial.print(i);
              Serial.print("///diff:");
              Serial.println(position_diff[i]);
            }*/
          }
        }
      }
    }
    for (int i = 1; i <= 7; i = i + 2) {
      if (position_input[i] != position_current[i] || turns_input[i] != 0) {
        digitalWrite(STEP[i], LOW);
        delayMicroseconds(1);
        digitalWrite(STEP[i], HIGH);
        if (forward2) {
          position_current[i]++;
        } else {
          position_current[i]--;
        }
        if (position_current[i] == STEPS) {
          position_current[i] = 0;
          if (turns_input[i] > 0) {
            turns_input[i]--;
          }
        } else if (position_current[i] == -1) {
          position_current[i] = STEPS - 1;
          if (turns_input[i] > 0) {
            turns_input[i]--;
          }
        }
      }
      if (analogRead(hall_sensors[i]) <= 500) {
        tempVal = 1;
      } else {
        tempVal = 0;
      }
      if (tempVal != hall_flags[i]) {
        hall_flags[i] = tempVal;
        hall_positions[i][tempVal] = position_current[i];
        if (tempVal == 0) {
          if ((hall_positions[i][0] > hall_positions[i][1]) != forward2) {
            position_diff[i] = STEPS / 2 - (hall_positions[i][0] + hall_positions[i][1]) / 2;
          } else {
            position_diff[i] = STEPS - (hall_positions[i][0] + hall_positions[i][1]) / 2;
          }
          if (forward2) {
            position_diff[i] += hall_delta[i];
          } else {
            position_diff[i] -= hall_delta[i];
          }
          if (position_diff[i] < 0) {
            position_diff[i] += STEPS;
          }
          position_diff[i] %= STEPS;
          if (position_diff[i] > 24 && position_diff[i] < STEPS - 24) {
            position_current[i] = (position_current[i] + position_diff[i]) % STEPS;
            hall_positions[i][0] = (hall_positions[i][0] + position_diff[i]) % STEPS;
            hall_positions[i][1] = (hall_positions[i][1] + position_diff[i]) % STEPS;
          }
        }
      }
    }

    //Serial.print(position_current[0]);
    //Serial.print("/");
    //Serial.println(led_current[0]);
  }
}
void DataReceive(int numBytes) {
  /*
  0-9:0-4.5
  a - b - c - d - e - f - g - h - i - j - k - l  - m - n
  5 -5.5- 6 -6.5- 7 -7.5- 8 -8.5- 9 -9.5-10 -10.5-11 -11.5 
  A-U: number of turns
  w,x,y,z: forward/backward
  v: find zeros
  */
  char rvdData[7];
  for (int i = 0; i < numBytes; i++) {
    rvdData[i] = Wire.read();
    if (rvdData[i] >= '0' && rvdData[i] <= '9') {
      position_input[i] = (rvdData[i] - '0') * STEPS / 24;
    } else if (rvdData[i] >= 'a' && rvdData[i] <= 'n') {
      position_input[i] = (rvdData[i] - 'a' + 10) * STEPS / 24;
    } else if (rvdData[i] >= 'A' && rvdData[i] <= 'U') {
      turns_input[i] = rvdData[i] - 'A';
    } else if (rvdData[i] == 'w') {
      forward1 = true;
      forward2 = true;
      digitalWrite(DIR_A, forward1 ? HIGH : LOW);
      digitalWrite(DIR_B, forward2 ? HIGH : LOW);
    } else if (rvdData[i] == 'x') {
      forward1 = true;
      forward2 = false;
      digitalWrite(DIR_A, forward1 ? HIGH : LOW);
      digitalWrite(DIR_B, forward2 ? HIGH : LOW);
    } else if (rvdData[i] == 'y') {
      forward1 = false;
      forward2 = true;
      digitalWrite(DIR_A, forward1 ? HIGH : LOW);
      digitalWrite(DIR_B, forward2 ? HIGH : LOW);
    } else if (rvdData[i] == 'z') {
      forward1 = false;
      forward2 = false;
      digitalWrite(DIR_A, forward1 ? HIGH : LOW);
      digitalWrite(DIR_B, forward2 ? HIGH : LOW);
    } else if (rvdData[i] == 'v') {
      zeroFlag = true;
      findZero();
    }
  }
}
void statusRequest() {
  bool statusFlag = true;
  for (int i = 0; i <= 7; i = i + 1) {
    if (position_input[i] != position_current[i] || turns_input[i] != 0) {
      statusFlag = false;
    }
  }
  Wire.write(statusFlag);
}
void findZero() {
  long stepSum[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  int stepNum[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  int tempVal;
  bool prePosition = true;
  while (prePosition) {
    prePosition = false;
    for (int i = 0; i <= 7; i = i + 2) {
      tempVal = digitalRead(hall_sensors[i]);
      if (tempVal == LOW) {
        digitalWrite(STEP[i], LOW);
        delayMicroseconds(1);
        digitalWrite(STEP[i], HIGH);
        prePosition = true;
      }
    }
    for (int i = 1; i <= 7; i = i + 2) {
      tempVal = analogRead(hall_sensors[i]);
      if (tempVal < 500) {
        digitalWrite(STEP[i], LOW);
        delayMicroseconds(1);
        digitalWrite(STEP[i], HIGH);
        prePosition = true;
      }
    }
    delay(200);
  }
  for (int j = 0; j < STEPS; j++) {
    for (int i = 0; i <= 7; i = i + 2) {
      tempVal = digitalRead(hall_sensors[i]);
      if (tempVal == LOW) {
        stepSum[i] = stepSum[i] + j;
        stepNum[i]++;
      }
      digitalWrite(STEP[i], LOW);
      delayMicroseconds(1);
      digitalWrite(STEP[i], HIGH);
    }
    for (int i = 1; i <= 7; i = i + 2) {
      tempVal = analogRead(hall_sensors[i]);
      if (tempVal < 500) {
        stepSum[i] = stepSum[i] + j;
        stepNum[i]++;
      }
      digitalWrite(STEP[i], LOW);
      delayMicroseconds(1);
      digitalWrite(STEP[i], HIGH);
    }
    delay(200);
  }

  for (int i = 0; i < 7; i = i + 2) {
    position_current[i] = forward1 ? (stepSum[i] / stepNum[i] - delta_A) : (STEPS - (stepSum[i] / stepNum[i] - delta_A));
    if (position_current[i] >= STEPS) {
      position_current[i] = position_current[i] - STEPS;
    }
    if (position_current[i] < 0) {
      position_current[i] = position_current[i] + STEPS;
    }
  }
  for (int i = 1; i <= 7; i = i + 2) {
    position_current[i] = forward2 ? (STEPS - (stepSum[i] / stepNum[i] - delta_B)) : (stepSum[i] / stepNum[i] - delta_B);
    if (position_current[i] >= STEPS) {
      position_current[i] = position_current[i] - STEPS;
    }
    if (position_current[i] < 0) {
      position_current[i] = position_current[i] + STEPS;
    }
  }
  zeroFlag = false;
}
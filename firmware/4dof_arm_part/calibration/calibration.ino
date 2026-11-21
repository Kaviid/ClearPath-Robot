#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver();
int current[4] = {90, 90, 90, 90};   // start all at 90°

void writeAngle(int ch, int angle) {
  int us = map(angle, 0, 180, 500, 2500);   // pulse width
  pca.writeMicroseconds(ch, us);
}

void moveSmooth(int ch, int target) {
  int step = (target > current[ch]) ? 1 : -1;
  while (current[ch] != target) {
    current[ch] += step;
    writeAngle(ch, current[ch]);
    delay(15);
  }
}

void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(50);
  for (int i = 0; i < 4; i++) writeAngle(i, 90);
  Serial.println("Type: channel angle  (e.g. 1 45)");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;          // ignore empty input

    int ch, angle;
    if (sscanf(line.c_str(), "%d %d", &ch, &angle) == 2 &&
        ch >= 0 && ch <= 3 && angle >= 0 && angle <= 180) {
      moveSmooth(ch, angle);
      Serial.print("CH"); Serial.print(ch);
      Serial.print(" -> "); Serial.println(angle);
    } else {
      Serial.println("Bad input: " + line);
    }
  }
}
#include <Servo.h>
Servo s, n;

int sPos = 90;   // current position of s
int nPos = 160;  // current position of n
int speed = 12;

void moveSmooth(Servo &servo, int &pos, int target, int speedDelay) {
  while (pos != target) {
    pos += (target > pos) ? 1 : -1;
    servo.write(pos);
    delay(speedDelay);   // bigger = slower
  }
}

void setup() {
  s.attach(9);
  n.attach(8);
  s.write(sPos);
  n.write(nPos);
  delay(500);
}

void loop() {
  moveSmooth(n, nPos, 120, 1); delay(1000);
  moveSmooth(n, nPos, 40, 1); delay(1000);

  moveSmooth(s, sPos, 60, speed);  delay(100);
  moveSmooth(s, sPos, 90, speed);  delay(100);
}
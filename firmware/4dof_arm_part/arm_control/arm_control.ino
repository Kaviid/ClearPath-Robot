// ClearPath Robot - Arm control (Phase 4: pose-based pick and clear)
// Channels: 0 = gripper, 1 = link 1 (shoulder), 2 = link 2 (elbow), 3 = base
// Serial Monitor: 9600 baud, "Newline"
//   p = run pick-and-clear    h = go home
//   1..6 = go to that pose    c = print current angles

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver();

// ---------- Limits (from notes/arm_specs.md) ----------
const int MIN_ANGLE[4] = {30, 40, 50, 0};     // gripper, link1, link2, base
const int MAX_ANGLE[4] = {80, 160, 150, 180};

const int GRIP_OPEN  = 80;
const int GRIP_CLOSE = 30;

// ---------- Poses: {gripper, link1, link2, base} ----------
// Tune these with the calibration sketch, then update here
const int HOME[4]    = {30, 120,  90,  90};
const int REACH[4]   = {80,  70, 120,  90};
const int GRIP[4]    = {30,  70, 120,  90};
const int LIFT[4]    = {30, 120,  90,  90};
const int TURN[4]    = {30, 120,  90, 160};
const int RELEASE[4] = {30,  70, 120, 160};   // lower first, gripper opens after

const int STEP_DELAY = 15;   // ms per degree; bigger = slower

int current[4] = {0, 40, 100, 90};   // start = HOME

// ---------- Low level ----------
void writeAngle(int ch, int angle) {
  int us = map(angle, 0, 180, 500, 2500);
  pca.writeMicroseconds(ch, us);
}

// Link 1 / link 2 coupling rule (measured)
bool isSafe(int s, int e) {
  if (s < MIN_ANGLE[1] || s > MAX_ANGLE[1]) return false;
  if (e < MIN_ANGLE[2] || e > MAX_ANGLE[2]) return false;
  int eMin = (s < 100) ? 100 - (s - 40) * 50 / 60 : 50;
  int eMax = (s <= 120) ? 150 : 150 - (s - 120) * 5 / 4;
  return (e >= eMin && e <= eMax);
}

// Move all 4 servos together, smoothly. Returns false if blocked.
bool moveTo(const int target[4]) {
  int t[4];
  for (int i = 0; i < 4; i++) t[i] = constrain(target[i], MIN_ANGLE[i], MAX_ANGLE[i]);

  if (!isSafe(t[1], t[2])) {
    Serial.println("UNSAFE target - not moving");
    return false;
  }

  int start[4];
  int steps = 0;
  for (int i = 0; i < 4; i++) {
    start[i] = current[i];
    steps = max(steps, abs(t[i] - start[i]));
  }

  for (int k = 1; k <= steps; k++) {
    int next[4];
    for (int i = 0; i < 4; i++) next[i] = start[i] + (long)(t[i] - start[i]) * k / steps;

    if (!isSafe(next[1], next[2])) {
      Serial.println("UNSAFE path - stopped");
      return false;
    }
    for (int i = 0; i < 4; i++) {
      current[i] = next[i];
      writeAngle(i, current[i]);
    }
    delay(STEP_DELAY);
  }
  return true;
}

void setGripper(int angle) {
  int p[4] = {angle, current[1], current[2], current[3]};
  moveTo(p);
}

// ---------- Sequence ----------
bool pickAndClear() {
  Serial.println("Pick and clear...");
  if (!moveTo(HOME))    return false;
  if (!moveTo(REACH))   return false;  delay(300);
  if (!moveTo(GRIP))    return false;  delay(500);
  if (!moveTo(LIFT))    return false;
  if (!moveTo(TURN))    return false;
  if (!moveTo(RELEASE)) return false;  delay(300);
  setGripper(GRIP_OPEN);               delay(500);
  if (!moveTo(LIFT))    return false;  // arm up again (base still turned)
  if (!moveTo(HOME))    return false;
  Serial.println("Done");
  return true;
}

void printCurrent() {
  Serial.print("G:");  Serial.print(current[0]);
  Serial.print(" L1:"); Serial.print(current[1]);
  Serial.print(" L2:"); Serial.print(current[2]);
  Serial.print(" B:");  Serial.println(current[3]);
}

// ---------- Setup / loop ----------
void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(50);
  delay(10);
  for (int i = 0; i < 4; i++) writeAngle(i, current[i]);
  Serial.println("Ready. p=pick, h=home, 1-6=pose, c=current");
}

void loop() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  char c = cmd.charAt(0);
  if      (c == 'p') pickAndClear();
  else if (c == 'h') moveTo(HOME);
  else if (c == '1') moveTo(HOME);
  else if (c == '2') moveTo(REACH);
  else if (c == '3') moveTo(GRIP);
  else if (c == '4') moveTo(LIFT);
  else if (c == '5') moveTo(TURN);
  else if (c == '6') moveTo(RELEASE);
  else if (c == 'c') printCurrent();
  else Serial.println("Unknown command");

  printCurrent();
}
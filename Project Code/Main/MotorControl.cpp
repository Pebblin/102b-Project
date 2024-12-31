#include <MotorControl.hpp>
#include <Arduino.h>

MotorControl::MotorControl(int a1, int a2, int b1, int b2) { // Constructor
  ain1 = a1;
  ain2 = a2;
  bin1 = b1;
  bin2 = b2;
}


// 1h 2l for CW, 1l 2h for CCW
void MotorControl::right() { // 1 CCW 2 CCW
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, HIGH);
}

void MotorControl::left() { // 1 CW 2 CW
  digitalWrite(ain1, HIGH);
  digitalWrite(ain2, LOW);

  digitalWrite(bin1, HIGH);
  digitalWrite(bin2, LOW);
}

void MotorControl::up() { // 1 CCW 2 CW
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);

  digitalWrite(bin1, HIGH);
  digitalWrite(bin2, LOW);
}

void MotorControl::down() { // 1 CCW 2 CW
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, HIGH);
}

void MotorControl::upright() { // 1 OFF 2 CCW
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, HIGH);
}

void MotorControl::upleft() { // 1 CCW 2 OFF
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);
}

void MotorControl::downright() { // 1 CW 2 OFF
  digitalWrite(ain1, HIGH);
  digitalWrite(ain2, LOW);

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);
}

void MotorControl::downleft() { // 1 OFF 2 CW
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);

  digitalWrite(bin1, HIGH);
  digitalWrite(bin2, LOW);
}

void MotorControl::off() { // Turn off motors
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);
}

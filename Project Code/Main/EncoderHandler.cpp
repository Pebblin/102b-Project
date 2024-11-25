#include <EncoderHandler.hpp>
#include <Arduino.h>


void handleEncoder1() {
  int currentAAState = digitalRead(encoderAAPin);
  int currentBAState = digitalRead(encoderBAPin);

  // Check transitions for encoder 1
  if (lastAAState == LOW && currentAAState == HIGH) {
    if (currentBAState == LOW) {
      encoderCount1++;
    } else {
      encoderCount1--;
    }
  } else if (lastAAState == HIGH && currentAAState == LOW) {
    if (currentBAState == HIGH) {
      encoderCount1++;
    } else {
      encoderCount1--;
    }
  }

  lastAAState = currentAAState;
}

void handleEncoder2() {
  int currentABState = digitalRead(encoderABPin);
  int currentBBState = digitalRead(encoderBBPin);

  // Check transitions for encoder 2
  if (lastABState == LOW && currentABState == HIGH) {
    if (currentBBState == LOW) {
      encoderCount2++;
    } else {
      encoderCount2--;
    }
  } else if (lastABState == HIGH && currentABState == LOW) {
    if (currentBBState == HIGH) {
      encoderCount2++;
    } else {
      encoderCount2--;
    }
  }

  lastABState = currentABState;
}

void resetEncoders() {
  encoderCount1 = 0;
  encoderCount2 = 0;
  lastAAState = digitalRead(encoderAAPin);
  lastABState = digitalRead(encoderABPin);
}
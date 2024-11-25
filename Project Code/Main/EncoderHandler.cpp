#include <EncoderHandler.hpp>
#include <Arduino.h>

EncoderHandler::EncoderHandler(int AA, int BA, int AB, int BB) { // Constructor
  encoderCount1 = 0;
  lastAAState = 0;

  encoderCount2 = 0;
  lastABState = 0;

  encoderAAPin = AA;
  encoderBAPin = BA;
  encoderABPin = AB;
  encoderBBPin = BB;

  cpr = 700;
}

void EncoderHandler::handleEncoder1() {
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

void EncoderHandler::handleEncoder2() {
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

void EncoderHandler::resetEncoders() {
  encoderCount1 = 0;
  encoderCount2 = 0;
  lastAAState = digitalRead(encoderAAPin);
  lastABState = digitalRead(encoderABPin);
}

int EncoderHandler::returnEncoder1() {
  return encoderCount1;
}

int EncoderHandler::returnEncoder2() {
  return encoderCount2;
}

int EncoderHandler::returnRotation1() {
  return (encoderCount1/cpr);
}

int EncoderHandler::returnRotation2() {
  return (encoderCount2/cpr);
}
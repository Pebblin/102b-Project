#include <ButtonHandler.hpp>
#include <Arduino.h>

ButtonHandler::ButtonHandler(int bp) { // Constructor
  buttonPin = bp;
  lastState = true;
  debounceDelay = 50;
  storage = true;
}

bool ButtonHandler::detectRisingEdge(bool temp) { // Detect change in button with built in debounce
  if (temp != storage) { // If state has changed, reset debounce timer
    debounceTimer = millis();
    // Serial.println("State Changed");
    storage = temp;
  }

  if ((millis() - debounceTimer) > debounceDelay && temp != lastState) { // Check if sufficient time has passed, execute code if yes
    // Serial.println("Sufficient Time Passed");
    if (!temp && lastState) { // If button was pressed, return true
      // Serial.println("Button Pressed");
      lastState = temp;
      return true;
    }

    // Serial.println("Button Released");

    lastState = temp;
  }

  // Serial.println("Checking Button Status");

  return false;
}
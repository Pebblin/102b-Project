#include <MotorControl.hpp>
#include <JoystickHandler.hpp>
#include <Arduino.h>

class MotorControl;

JoystickHandler::JoystickHandler(int xp, int yp) { // Constructor
  xpin = xp;
  ypin = yp;
}

void JoystickHandler::cardinalInput(MotorControl& MC) {
  int xaxis = analogRead(xpin);
  int yaxis = analogRead(ypin);

  int xval = map(xaxis, 0, 4095, -512, 512);
  int yval = map(yaxis, 0, 4095, -512, 512);

  // Enable deadzone
  if (abs(xval) < 100 && abs(yval) < 100) {
    // Serial.println("Joystick idle ");
    MC.off();

    return;
  }

  // Check joystick movement and call corresponding movement functions
  if (abs(xval) >= abs(yval)) { // Override y input if x > y
    if (xval > 100) {
      MC.right();
      // Serial.print("x: ");
      // Serial.println(xval);
    
    } else if (xval < -100) {
      MC.left();
      // Serial.print("x: ");
      // Serial.println(xval);
      
    }
  } else { // Override x input if y > x
    if (yval > 100) { // Note that y is flipped (positive is down, negative up) can change if we want
      MC.down();
      // Serial.print("y: ");
      // Serial.println(yval);

    } else if (yval < -100) {
      MC.up();
      // Serial.print("y: ");
      // Serial.println(yval);

    }
  }
}

void JoystickHandler::diagonalInput(MotorControl& MC) {
  int xaxis = analogRead(xpin);
  int yaxis = analogRead(ypin);

  int xval = map(xaxis, 0, 4095, -512, 512);
  int yval = map(yaxis, 0, 4095, -512, 512);

  // Enable deadzone
  if (abs(xval) < 100 && abs(yval) < 100) {
    // Serial.println("Joystick idle ");
    MC.off();

    return;
  }

  // Check joystick movement and call corresponding movement functions
  if (xval > 0) { // Moving right
    if (yval > 0) { // Moving down
      // Serial.println("Diagonal down right")
      MC.downright();
    } else {
      // Serial.println("Diagonal up right")
      MC.upright();
    }

  } else { // Moving left
    if (yval > 0) { // Moving down
      // Serial.println("Diagonal down left")
      MC.downleft();

    } else { // Moving up
      // Serial.println("Diagonal up left"
      MC.upleft();

    }
  }
}
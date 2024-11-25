#include <MotorControl.hpp>
#include <JoystickHandler.hpp>
#include <EncoderHandler.hpp>
#include <ButtonHandler.hpp>

// Define pin constants
const int manualButton = 27;    // Define button for manual control/pattern toggle
const int diagCardButton = 32;  // Define button for diagonal/cardinal directionality
const int xpin = 13;            // Define x-axis pin
const int ypin = 12;            // Define y-axis pin
const int ain1 = 26;            // Pin 1 for motor A
const int ain2 = 25;            // Pin 2 for motor A
const int bin1 = 27;            // Pin 1 for motor B
const int bin2 = 33;            // Pin 2 for motor B
const int encoderAAPin = 26;    // Encoder A pin for motor A
const int encoderBAPin = 25;    // Encoder B pin for motor A
const int encoderABPin = 32;    // Encoder A pin for motor B
const int encoderBBPin = 14;    // Encoder B pin for motor B
const int vlimButton = 0;       // Vertical limit switch pin
const int hlimButton = 0;       // Horizontal limit switch pin
const int canvasLimit = 1;      // Maximum canvas bounds

// To convert encoder ticks to revolutions
const int countsPerRevolution = 700;  // 700 encoder ticks per revolution for our motor

// Define booleans
bool toggleControl = true;      // Checks if button was pressed
bool toggleCardinality = true;  // Checks if was last in cardinal control or diagonal control
bool homingSequence = false;    // Checks if the homing sequence is currently running
bool verticalLimit = false;     // Checks if the vertical limit switch was pressed
bool horizontalLimit = false;   // Checks if the horizontal limit switch was pressed

// Create class objects
MotorControl motCon(ain1, ain2, bin1, bin2);
JoystickHandler joyHan(xpin, ypin);
ButtonHandler manualControl(manualButton);
ButtonHandler diagCardControl(diagCardButton);
ButtonHandler verticalLim(vlimButton);
ButtonHandler horizontalLim(hlimButton);

// Limit switch interrupt functions
void IRAM_ATTR vlim() {         // Vertical limit switch function
  if (verticalLim.detectRisingEdge(digitalRead(vlimButton))) { // Debounce for limit switch
    verticalLimit = true;       // Change state
  }
}

void IRAM_ATTR hlim() {         // Horizontal limit switch function
  if (horizontalLim.detectRisingEdge(digitalRead(hlimButton))) { // Debounce for limit switch
    horizontalLimit = true;     // Change state
  }
}

void setup() {
  Serial.begin(115200);                             // Start serial communication
  pinMode(manualButton, INPUT_PULLUP);              // Setup manual control button pin with pull-up
  pinMode(diagCardButton, INPUT_PULLUP);            // Setup diagonal/cardinal control pin with pull-up
  pinMode(xpin, INPUT);                             // Setup x-axis pin as input
  pinMode(ypin, INPUT);                             // Setup y-axis pin as input

  pinMode(ain1, OUTPUT);                            // Setup motor control pins as outputs
  pinMode(ain2, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);

  attachInterrupt(vlimButton, vlim, INPUT_PULLUP);  // Limit button interrupt functions
  attachInterrupt(hlimButton, hlim, INPUT_PULLUP);  

  attachInterrupt(encoderAAPin, handleEncoder1, CHANGE);  // Encoder pin interrupt functions
  attachInterrupt(encoderBAPin, handleEncoder1, CHANGE);
  attachInterrupt(encoderABPin, handleEncoder2, CHANGE);
  attachInterrupt(encoderBBPin, handleEncoder2, CHANGE);

  digitalWrite(ain1, LOW);                          // Turn off all motors
  digitalWrite(ain2, LOW);
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW); 
}

void loop() {
  while (homingSequence) {                  // Run homing sequence while true
    homing(motCon);
  }

  if (toggleControl) {                      // If manual control has been enabled, run manual control
    if (toggleCardinality) {                // If true, enable cardinal controls
      joyHan.cardinalInput(motCon);
      Serial.println("Currently accepting cardinal inputs");

    } else {
      joyHan.diagonalInput(motCon);
      Serial.println("Currently accepting diagonal inputs");

    }

    if (diagCardControl.detectRisingEdge(digitalRead(diagCardButton))) { // Only check diag/card change while in manual mode
    toggleCardinality = !toggleCardinality; // Change state
    }

  } else {  // Else loop pattern
    if (homingSequence) {                   // If homingsequence is currently running, do not begin pattern mode (here for redundancy)
      Serial.println("Uh oh.");             // This code block should in theory never be triggered
      return;
    }
    motCon.off();
    Serial.println("Pattern Mode");

  }
  
  if (manualControl.detectRisingEdge(digitalRead(manualButton))) { // If button has been pressed, change control mode
    if (toggleControl) {                    // If is currently in manual mode and changing to pattern mode
      homingSequence = true;                // Begin homing sequence and toggle pattern mode
      toggleControl = !toggleControl;
    }

    if (homingSequence) {                   // If homing sequence is currently running, do not allow for mode changes (here for redundancy)
      Serial.println("Oh no.");             // This code block should in theory never be triggered
      return;

    } else {
      toggleControl = !toggleControl;       // Change state

    }
  }
}

// Homing function
void homing(MotorControl& MC) {
  if (!verticalLimit) {     // Don't continue until vertical limit has been reached
    MC.down();              // Move motors down
    return;
  }

  if (!horizontalLimit) {   // Don't continue until horizontal limit has been reached
    MC.left();              // Move motors down
    return;
  }

  MC.off();

  //void resetEncoders();
  // Move carriage x and y encoder ticks right and up
  homingSequence = false;   // Exit loop
}
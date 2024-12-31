#include <MotorControl.hpp>
#include <JoystickHandler.hpp>
#include <EncoderHandler.hpp>
#include <ButtonHandler.hpp>

// Define pin constants
const int manualButton = 15;    // Define button for manual control/pattern toggle
const int diagCardButton = 4;   // Define button for diagonal/cardinal directionality
const int xpin = 34;            // Define x-axis pin
const int ypin = 39;            // Define y-axis pin
const int ain1 = 12;            // Pin 1 for motor A
const int ain2 = 13;            // Pin 2 for motor A
const int bin1 = 33;            // Pin 1 for motor B
const int bin2 = 27;            // Pin 2 for motor B
const int AAPin = 32;           // Encoder A pin for motor A
const int BAPin = 14;           // Encoder B pin for motor A
const int ABPin = 26;           // Encoder A pin for motor B
const int BBPin = 25;           // Encoder B pin for motor B
const int vlimButton = 20;      // Vertical limit switch pin
const int hlimButton = 22;      // Horizontal limit switch pin


int state = 1;
int enc = 0;

// Define booleans
bool toggleControl = true;      // Checks if button was pressed
bool toggleCardinality = false;  // Checks if was last in cardinal control or diagonal control
bool homingSequence = false;    // Checks if the homing sequence is currently running
bool verticalLimit = false;     // Checks if the vertical limit switch was pressed
bool horizontalLimit = false;   // Checks if the horizontal limit switch was pressed

// Create class objects
MotorControl motCon(ain1, ain2, bin1, bin2);
JoystickHandler joyHan(xpin, ypin);
ButtonHandler manualControl(manualButton);
ButtonHandler diagCardControl(diagCardButton);
// ButtonHandler vlim(vlimButton);
// ButtonHandler hlim(hlimButton);
EncoderHandler encoderHandler(AAPin, BAPin, ABPin, BBPin);

// Encoder interrupt functions
void IRAM_ATTR enc1() {         // Encoder updater 1
  encoderHandler.handleEncoder1();
}

void IRAM_ATTR enc2() {         // Encoder updater 2
  encoderHandler.handleEncoder2();
}

// void IRAM_ATTR manual() {    // Check for manual control (debugging)
//   Serial.println("Manual control button pressed");
// }

// void IRAM_ATTR direction() { // Check for direction toggle (debugging)
//   Serial.println("Joystick button pressed");
// }

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

  pinMode(vlimButton, INPUT_PULLUP);                // Limit switches with pull-up
  pinMode(hlimButton, INPUT_PULLUP);

  pinMode(AAPin, INPUT_PULLUP);                     // Activate encoder pins
  pinMode(BAPin, INPUT_PULLUP);
  pinMode(ABPin, INPUT_PULLUP);
  pinMode(BBPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(AAPin), enc1, CHANGE);             // Encoder pin interrupt functions
  attachInterrupt(digitalPinToInterrupt(BAPin), enc1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ABPin), enc2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BBPin), enc2, CHANGE);

  // attachInterrupt(digitalPinToInterrupt(manualButton), manual, CHANGE); // Debugging purposes
  // attachInterrupt(digitalPinToInterrupt(diagCardButton), direction, CHANGE);

  digitalWrite(ain1, LOW);                          // Turn off all motors
  digitalWrite(ain2, LOW);
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW); 

  // Serial.println("Setup successful!");
}

void loop() {
  // Serial.println("New loop initiated");

  while (homingSequence) {                  // Run homing sequence while true
    if (manualControl.detectRisingEdge(digitalRead(manualButton))) {
      toggleControl = true;
      homingSequence = false;
      state = 1;
      return;
    }

    if (!(digitalRead(vlimButton))) {
      // Serial.println("Vertical limit reached");
      verticalLimit = true;
    }

    if (!(digitalRead(hlimButton))) {
      // Serial.println("Horizontal limit reached");
      horizontalLimit = true;
    }
    homing(motCon);                         // Do not go to switch statement until homing sequence is done
    encoderHandler.resetEncoders();
    enc = encoderHandler.returnEncoder1();
  }

  // if (!(digitalRead(vlimButton))) {
  //   Serial.println("Vertical limit reached");
  // }

  // if (!(digitalRead(hlimButton))) {
  //   Serial.println("Horizontal limit reached");
  // }

  if (diagCardControl.detectRisingEdge(digitalRead(diagCardButton))) {
    // Serial.println("State changed");
    toggleCardinality = !toggleCardinality;   // Change state
  }

  if (manualControl.detectRisingEdge(digitalRead(manualButton))) { // If button has been pressed, enable standby mode
    // Serial.println("State changed");
    if (toggleControl) {                      // If was true, enable homing sequence
      homingSequence = true;
      verticalLimit = false;
      horizontalLimit = false;
      toggleControl = false;
      toggleCardinality = false;
    } else {
      toggleControl = true;
    }
  }

  // Serial.println(digitalRead(hlimButton));
  // Serial.println(digitalRead(vlimButton));

  switch (state) { // bottom right = 10700 enc 1
    case 0:   // Standby mode enabled
      if (homingSequence) {                   // If homingsequence is currently running, do not begin pattern mode (here for redundancy)
        Serial.println("Uh oh");              // This code block should in theory never be triggered
        homingSequence = false;
        break;
      } 

      pattern(motCon);

      motCon.off();
      // Serial.println("Standby Mode");
      toggleCardinality = false;

      state = toggleControl + toggleCardinality;
      break;
    case 1:   // Standby mode disabled / Cardinal manual mode
      joyHan.cardinalInput(motCon);
      // Serial.println(encoderHandler.returnEncoder1());
      Serial.println("Currently accepting cardinal inputs");

      // if (verticalLimit) {
      //   Serial.println("Vertical limit has been reached");
      // }

      // if (horizontalLimit) {
      //   Serial.println("Horizontal limit has been reached");
      // }
      state = toggleControl + toggleCardinality;
      break;
    case 2:   // Diagonal manual mode
      joyHan.diagonalInput(motCon);
      Serial.println("Currently accepting diagonal inputs");

      // if (verticalLimit) {
      //   Serial.println("Vertical limit has been reached");
      // }

      // if (horizontalLimit) {
      //   Serial.println("Horizontal limit has been reached");
      // }

      state = toggleControl + toggleCardinality;
      break;
  }
}

// Homing function
void homing(MotorControl& MC) {
  
  if (!verticalLimit) {                     // Don't continue until vertical limit has been reached
    // Serial.println("Approaching vertical limit");
    MC.down();                              // Move motors down
    return;
  }

  if (!horizontalLimit) {                   // Don't continue until horizontal limit has been reached
    // Serial.println("Approaching horizontal limit");
    MC.left();                              // Move motors down
    return;
  }

  MC.off();

  homingSequence = false;                   // Exit loop
  verticalLimit = false;
  horizontalLimit = false;
}

void pattern(MotorControl& MC) { 
  while (encoderHandler.returnEncoder1() < 10500) {
    enc = encoderHandler.returnEncoder1();

    while (encoderHandler.returnEncoder1() > enc - 10800) { // Move up
      if (manualControl.detectRisingEdge(digitalRead(manualButton))) {
      toggleControl = true;
      return;
      }

      MC.up();
    }

    enc = encoderHandler.returnEncoder1();

    while (encoderHandler.returnEncoder1() < enc + 1000) {
      if (manualControl.detectRisingEdge(digitalRead(manualButton))) {
        toggleControl = true;
        return;
      }

      MC.right();
    }

    while (digitalRead(vlimButton)) {
      if (manualControl.detectRisingEdge(digitalRead(manualButton))) {
        toggleControl = true;
        return;
      }

      MC.down();
    }

    enc = encoderHandler.returnEncoder1();

    while (encoderHandler.returnEncoder1() < enc + 1000) {
      if (manualControl.detectRisingEdge(digitalRead(manualButton))) {
        toggleControl = true;
        return;
      }

      MC.right();
    }
  }

  homingSequence = true; // If the end of the pattern is reached, initiate homing sequence again
}
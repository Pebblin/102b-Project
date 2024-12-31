#include <MotorControl.hpp>
#include <JoystickHandler.hpp>
#include <EncoderHandler.hpp>

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


int state = 0;

// Define booleans
bool toggleControl = true;      // Checks if button was pressed
bool toggleCardinality = false; // Checks if was last in cardinal control or diagonal control
bool homingSequence = false;    // Checks if the homing sequence is currently running
bool verticalLimit = false;     // Checks if the vertical limit switch was pressed
bool horizontalLimit = false;   // Checks if the horizontal limit switch was pressed
bool checkControl = false;      // Enables debounce and begins service
bool checkCardinality = false;  // Enables debounce and begins service

hw_timer_t* timer = NULL;

// Create class objects
MotorControl motCon(ain1, ain2, bin1, bin2);
JoystickHandler joyHan(xpin, ypin);
EncoderHandler encoderHandler(AAPin, BAPin, ABPin, BBPin);

// Encoder interrupt functions
void IRAM_ATTR enc1() {         // Encoder updater 1
  encoderHandler.handleEncoder1();
}

void IRAM_ATTR enc2() {         // Encoder updater 2
  encoderHandler.handleEncoder2();
}

void IRAM_ATTR manual() {       // Check for manual control
  // Serial.println("Manual control button pressed");
  checkControl = !checkControl;
}

void IRAM_ATTR direction() {    // Check for direction toggle 
  // Serial.println("Joystick button pressed");
  checkCardinality = !checkCardinality;
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

  attachInterrupt(digitalPinToInterrupt(manualButton), manual, FALLING);
  attachInterrupt(digitalPinToInterrupt(diagCardButton), direction, FALLING);

  digitalWrite(ain1, LOW);                          // Turn off all motors
  digitalWrite(ain2, LOW);
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);

  timer = timerBegin(1000000);
  timerStop(timer);
  timerRestart(timer); 

  // Serial.println("Setup successful!");
}

void loop() {
  // Serial.println("New loop initiated");

  while (homingSequence) {                  // Run homing sequence while true
    if (!(digitalRead(vlimButton))) {
      verticalLimit = true;
    }

    if (!(digitalRead(hlimButton))) {
      horizontalLimit = true;
    }
    homing(motCon);                         // Do not go to switch statement until homing sequence is done
    encoderHandler.resetEncoders();
  }

  // if (!(digitalRead(vlimButton))) {
  //   Serial.println("Vertical limit reached");
  // }

  // if (!(digitalRead(hlimButton))) {
  //   Serial.println("Horizontal limit reached");
  // }

  if (checkForButtonPress(checkCardinality)) {                     // Event checker
    Serial.println("Cardinality changed");
    toggleCardinality = !toggleCardinality; // Change state

    checkCardinality = false;
  }

  if (checkForButtonPress(checkControl)) {                         // Event checker
    Serial.println("Control changed");
    if (toggleControl) {                    // If was true, enable homing sequence
      homingSequence = true;
      verticalLimit = false;
      horizontalLimit = false;
      toggleControl = false;
      toggleCardinality = false;
    } else {
      toggleControl = true;
    }
    checkControl = false;
  }

  // Serial.println(digitalRead(hlimButton));
  // Serial.println(digitalRead(vlimButton));

  switch (state) {  // Top left = -11800 bottom right = 10700 enc 1 // top left =
    case 0:   // Standby mode enabled
      if (homingSequence) {                   // If homingsequence is currently running, do not begin pattern mode (here for redundancy)
        Serial.println("Uh oh");              // This code block should in theory never be triggered
        homingSequence = false;
        break;
      } 

      // pattern(motCon);

      motCon.off();
      Serial.println("Standby Mode");
      toggleCardinality = false;

      state = toggleControl + toggleCardinality;
      break;
    case 1:   // Standby mode disabled / Cardinal manual mode
      joyHan.cardinalInput(motCon);
      Serial.println("Currently accepting cardinal inputs");
      // Serial.println(encoderHandler.returnEncoder1());


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

bool checkForButtonPress(bool variable) {
  if (variable == true) {
    timerStart(timer);
    while (timerReadMillis(timer) < 200) {
      continue;
    }
    timerStop(timer);
    timerRestart(timer);
    return true;
  }
  return false;
}

void pattern(MotorControl& MC) { // To be implemented
  while (encoderHandler.returnEncoder1() > -11000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -10500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 1000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -10000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -9500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 1500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 2000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -9000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -8500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 2500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 3000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -8000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -7500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 3500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 4000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -7000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -6500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 4500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 5000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -6000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -5500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 5500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 6000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -5000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -4500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 6500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 7000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -4000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -3500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 7500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 8000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -4000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -3500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 8500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }

  while (encoderHandler.returnEncoder1() < 9000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() > -3000) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.up();
  }

  while (encoderHandler.returnEncoder1() < -2500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.right();
  }

  while (encoderHandler.returnEncoder1() < 9500) {
    if (checkForButtonPress(manualButton)) {
      toggleControl = true;
      return;
    }
    MC.down();
  }
  homingSequence = true; // If the of the pattern is reached, initiate homing sequence again
}
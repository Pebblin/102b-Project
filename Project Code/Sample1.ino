// Assign pins
const int button = 27; // Define the button pin
const int xpin = 13;  // Define x-axis pin
const int ypin = 12;  // Define y-axis pin
const int ain1 = 26; // high for clockwise motor1
const int ain2 = 25; // high for anticlockwise motor2
const int bin1 = 15;
const int bin2 = 33;

// Assign variables
int xaxis = 0;        // Variable to store x-axis value
int yaxis = 0;        // Variable to store y-axis value
int pressCount = 0;    // Count of button presses
bool lastState = HIGH; // Stores the previous state of the button
int btnval = HIGH;     // Current state of the button (renamed to btnval)

// Function prototypes
void right();
void left();
void up();
void down();
void manual();
void pushbutton();
void pattern();

void setup() {
  Serial.begin(115200);        // Start serial communication
  pinMode(button, INPUT_PULLUP); // Setup button pin with pull-up
  pinMode(xpin, INPUT);        // Setup x-axis pin as input
  pinMode(ypin, INPUT);        // Setup y-axis pin as input

  // Set motor control pins as OUTPUT
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);

  // Ensure motor is off at startup
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW); 
}


void loop() {
  pushbutton();
}

void pushbutton() {
  btnval = digitalRead(button); // Read the current state of the button

  // Check for a transition from HIGH (not pressed) to LOW (pressed)
  if (lastState == HIGH && btnval == LOW) {
    pressCount++; // Increment the press count
    delay(200); // Simple debounce delay
  }
  if (pressCount % 2 == 0) {
    pattern();
  } else {
    manual();
  }

  // Update the last state for the next loop iteration
  lastState = btnval;
}

void homing(){
  //first rotate motor to the bottom first until it presses the button. 
  //set y value to 0
  //then rotate motor to the left until it presses the button.
  //set x value to 0

  //define this as start position
}

void pattern() {
  Serial.println("Pattern mode");
}

void manual() {
  xaxis = analogRead(xpin);
  int xvalue = map(xaxis, 0, 4095, -512, 512);
  yaxis = analogRead(ypin);
  int yvalue = map(yaxis, 0, 4095, -512, 512);

  if (abs(xvalue) < 100 && abs(yvalue) < 100) {
    Serial.println("Joystick idle ");
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, LOW);
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, LOW);
    return;
  }

  if (abs(xvalue) >= abs(yvalue)) {
    if (xvalue > 100) {
      right();
      Serial.print("x: ");
      Serial.println(xvalue);
    
    } else if (xvalue < -100) {
      left();
      Serial.print("x: ");
      Serial.println(xvalue);
      
    }
  } else {
    if (yvalue > 100) {
      down();
      Serial.print("y: ");
      Serial.println(yvalue);
      
    } else if (yvalue < -100) {
      up();
      Serial.print("y: ");
      Serial.println(yvalue);
      
    }
  }

  delay(100);
}

void right() {
  motor1CW();
  motor2CW();
  Serial.println("right");
}

void left() {
  motor1CCW();
  motor2CCW();
  Serial.println("left");
}

void up() {
  motor1CCW();
  motor2CW();
  Serial.println("up");
}

void down() {
  motor1CW();
  motor2CCW();
  Serial.println("down");
}

void motor1CW() {

  digitalWrite(ain1, HIGH);
  digitalWrite(ain2, LOW);
}

void motor1CCW() {

  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);
}

void motor2CW() {

  digitalWrite(bin1, HIGH);
  digitalWrite(bin2, LOW);
}

void motor2CCW() {

  digitalWrite(bin1, LOW);
  digitalWrite(bin2, HIGH);
}
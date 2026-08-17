const int encoderPinA = 2; // Interrupt pin
const int encoderPinB = 4; // Direction pin

const int pinF = 7;
const int pinB = 6;

static int speed = 50;
static char lastKey = "";

// 0 stop
// 1 forward
// 2 back
static int lastMove = 0; 

volatile long encoderValue = 0;
long lastEncoderValue = -1;
int lastStateA;


// --- CHANGE THIS VALUE TO MATCH YOUR MOTOR'S CPR ---
const int CPR = 186;      // total count on hall effect encoder for one rotation
const int targetRotations = 2;
const long targetCounts = CPR * targetRotations;

void updateEncoderA() {

  int stateB = digitalRead(encoderPinB);
  if (lastMove == 1) {
    encoderValue++; // Clockwise
  } else if (lastMove == 2) {
    encoderValue--; 
  }

}

void updateEncoderB() {

  int stateA = digitalRead(encoderPinA);
  if (lastMove == 1) {
    encoderValue++; // Clockwise
  } else if (lastMove == 2) {
    encoderValue--; 
  }
}

void setup() {


  Serial.begin(9600);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  
  lastStateA = digitalRead(encoderPinA);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoderB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoderA, CHANGE);

  // Configure GPIO pins as outputs
  pinMode(pinF, OUTPUT);
  pinMode(pinB, OUTPUT);
  
  // Ensure pins start 0
  stopMotor();
  Serial.println("w for forward drive, e for backward, s for stop");
  Serial.println("> increase speed by 20, < decrease speed by 20");

}


void stopMotor() {
  analogWrite(pinF, 0);
  analogWrite(pinB, 0);
  encoderValue = 0;


}

void handleSerialInput() {
    if (Serial.available() > 0) {
    // Read the incoming byte
    char incomingChar = Serial.read();
    
    
    // Process input
    if (incomingChar == 'w') {
      lastMove = 1;
      Serial.println("Forward enabled");
    } 
    else if (incomingChar == 'e') {
      lastMove = 2;
      Serial.println("Backward enabled");
    }
    else if (incomingChar == 's') {
      lastMove = 0;
      Serial.println("Stop enabled");
    } else if (incomingChar == ',') {
      
      speed -= 20;
      Serial.print("Reduce speed");
      Serial.println(speed);

      if (speed < 0) speed = 0;
    } else if (incomingChar == '.') {

      speed += 20;  
      Serial.print("Increase speed ");
      Serial.println(speed);

      if (speed > 220) speed = 220;
    }

    if (lastMove == 0) {
      stopMotor();
    } else if (lastMove == 1) {
      // move forward
      analogWrite(pinB, 0);
      analogWrite(pinF, speed);
      
      Serial.print("Move forward ");
      Serial.println(speed);
    } else if (lastMove == 2) {
      // move forward
      analogWrite(pinF, 0);
      analogWrite(pinB, speed);
      
      Serial.print("Move backward ");
      Serial.println(speed);
    }

  }
}

void loop() {
  if (lastEncoderValue != encoderValue) {
    Serial.print("Encoder Count: ");
    Serial.println(encoderValue);
    lastEncoderValue = encoderValue;
  }
  // Check if target is met (accounts for potential minor overshoot)
  if (abs(encoderValue) >= targetCounts) {
    stopMotor();
    Serial.println("Target reached! Motor stopped.");
  }
  handleSerialInput();
  delay(10);
}


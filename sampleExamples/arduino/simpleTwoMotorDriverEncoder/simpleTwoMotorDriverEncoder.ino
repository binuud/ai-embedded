const int encoder1PinA = 2; // Interrupt pin
const int encoder1PinB = 2; // Direction pin

const int encoder2PinA = 3; // Interrupt pin
const int encoder2PinB = 3; // Direction pin // since arduino has only 2 interruptable pins

const int motor1F = 5;
const int motor1B = 6;

const int motor2F = 10;
const int motor2B = 11;

static int speed = 50;
static char lastKey = "";

// 0 stop
// 1 forward
// 2 back
static int lastMove = 0; 

volatile long encoder1Value = 0;
long lastEncoder1Value = -1;

volatile long encoder2Value = 0;
long lastEncoder2Value = -1;


// --- CHANGE THIS VALUE TO MATCH YOUR MOTOR'S CPR ---
const int CPR = 186;      // total count on hall effect encoder for one rotation
const int targetRotations = 2;
const long targetCounts = CPR * targetRotations;

void updateEncoder1A() {


  if (lastMove == 1) {
    encoder1Value++; // Clockwise
  } else if (lastMove == 2) {
    encoder1Value--; 
  }

}

void updateEncoder1B() {


  if (lastMove == 1) {
    encoder1Value++; // Clockwise
  } else if (lastMove == 2) {
    encoder1Value--; 
  }
}

void updateEncoder2A() {


  if (lastMove == 1) {
    encoder2Value++; // Clockwise
  } else if (lastMove == 2) {
    encoder2Value--; 
  }

}

void updateEncoder2B() {


  if (lastMove == 1) {
    encoder2Value++; // Clockwise
  } else if (lastMove == 2) {
    encoder2Value--; 
  }
}

void setup() {


  Serial.begin(9600);

  pinMode(encoder1PinA, INPUT);
  pinMode(encoder1PinB, INPUT);

  pinMode(encoder2PinA, INPUT);
  pinMode(encoder2PinB, INPUT);
  

  attachInterrupt(digitalPinToInterrupt(encoder1PinA), updateEncoder1A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder1PinB), updateEncoder1B, CHANGE);

  attachInterrupt(digitalPinToInterrupt(encoder2PinA), updateEncoder2A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder2PinB), updateEncoder2B, CHANGE);


  // Configure GPIO pins as outputs
  pinMode(motor1F, OUTPUT);
  pinMode(motor1B, OUTPUT);

  pinMode(motor2F, OUTPUT);
  pinMode(motor2B, OUTPUT);
  
  // Ensure pins start 0
  stopAll();
  Serial.println("w for forward drive, e for backward, s for stop");
  Serial.println("> increase speed by 20, < decrease speed by 20");

}



void stopMotor1() {

  analogWrite(motor1F, 0);
  analogWrite(motor1B, 0);

  encoder1Value = 0;

}

void stopMotor2() {

  analogWrite(motor2F, 0);
  analogWrite(motor2B, 0);
  encoder2Value = 0;

}

void stopAll() {
  stopMotor1();
  stopMotor2();
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
      stopAll();
    } else if (lastMove == 1) {
      // move forward
      analogWrite(motor1B, 0);
      analogWrite(motor1F, speed);

      analogWrite(motor2B, 0);
      analogWrite(motor2F, speed);
      
      Serial.print("Move forward ");
      Serial.println(speed);
    } else if (lastMove == 2) {
      // move forward
      analogWrite(motor1F, 0);
      analogWrite(motor1B, speed);

      analogWrite(motor2F, 0);
      analogWrite(motor2B, speed);
      
      Serial.print("Move backward ");
      Serial.println(speed);
    }

  }
}

void loop() {
  if (lastEncoder1Value != encoder1Value) {
    Serial.print("Encoder1 Count: ");
    Serial.print(encoder1Value);
    Serial.print(",Encoder2 Count: ");    Serial.println(encoder2Value);
    lastEncoder1Value = encoder1Value;
  }
  // Check if target is met (accounts for potential minor overshoot)
  if (abs(encoder1Value) >= targetCounts) {
    stopMotor1();
    Serial.print("Encoder Count: ");
    Serial.println(encoder1Value);
    Serial.println("Target reached! Motor1 stopped.");
  }
  if (abs(encoder2Value) >= targetCounts) {
    stopMotor2();
    Serial.print("Encoder Count: ");
    Serial.println(encoder2Value);
    Serial.println("Target reached! Motor2 stopped.");
  }
  handleSerialInput();
  delay(10);
}


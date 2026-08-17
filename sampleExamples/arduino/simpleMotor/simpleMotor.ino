const int pinF = 7;
const int pinB = 6;

static int speed = 0;
static char lastKey = "";

// 0 stop
// 1 forward
// 2 back
static int lastMove = 0; 

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
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
}

void loop() {
  // Check if data is available
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
      Serial.println("Reduce speed");
      speed -= 20;
      if (speed < 0) speed = 0;
    } else if (incomingChar == '.') {
      Serial.println("Increase speed");
      speed += 20;
      if (speed > 220) speed = 220;
    }
    if (lastMove == 0) {
      stopMotor();
    } else if (lastMove == 1) {
      // move forward
      analogWrite(pinF, speed);
      analogWrite(pinB, 0);
    } else if (lastMove == 2) {
      // move forward
      analogWrite(pinB, speed);
      analogWrite(pinF, 0);
    }

  }
}
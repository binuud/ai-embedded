const int encoderPinA = 2; // Interrupt pin
const int encoderPinB = 4; // Direction pin

volatile long encoderValue = 0;
long lastEncoderValue = -1;
int lastStateA;

void updateEncoderA() {

  int stateB = digitalRead(encoderPinB);
  if (stateB == HIGH) {
    encoderValue++; // Clockwise
  } else {
    encoderValue--; // Counter-clockwise
  }
}

void updateEncoderB() {

  int stateA = digitalRead(encoderPinA);
  if (stateA == HIGH) {
    encoderValue++; // Clockwise
  } else {
    encoderValue--; // Counter-clockwise
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  
  lastStateA = digitalRead(encoderPinA);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoderB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoderA, CHANGE);

}

void loop() {
  if (lastEncoderValue != encoderValue) {
    Serial.print("Encoder Count: ");
    Serial.println(encoderValue);
    lastEncoderValue = encoderValue;
  }

  delay(100);
}


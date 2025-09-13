const int PIRPin = 13;  // Connect HC-SR505 output to GPIO 13


void setup() {
  Serial.begin(115200);
  pinMode(PIRPin, INPUT);
  Serial.println("HC-SR505/501 PIR Motion Sensor Test");
}

void loop() {
  int motionState = digitalRead(PIRPin);

  if (motionState == HIGH) {
    Serial.println("Motion detected!");
  } else {
    Serial.println("No motion.");
  }

  delay(1000);  // Check every second
}

const int soundSensorPin = 13;  // Digital pin connected to LM393 output
const int ledPin = 2;         // Built-in LED pin on Arduino

void setup() {
  pinMode(soundSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("LM393 Sound Sensor Test");
}

void loop() {
  int soundState = digitalRead(soundSensorPin);

  if (soundState == HIGH) {
    digitalWrite(ledPin, HIGH);  // Turn on LED when sound detected
    Serial.println("Sound Detected");
  } else {
    digitalWrite(ledPin, LOW);   // Turn off LED when no sound detected
    Serial.println("No Sound");
  }

  delay(200);
}
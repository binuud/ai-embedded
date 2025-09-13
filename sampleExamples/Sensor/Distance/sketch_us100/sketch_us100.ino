const int trigPin = 21;  // Connect to US-100 Trig/TX pin
const int echoPin = 22;  // Connect to US-100 Echo/RX pin

long duration;
int distanceCm;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
}

void loop() {
  // Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the pulse duration on the echo pin
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  // Sound speed formula: distance (cm) = duration * 0.034 / 2
  distanceCm = duration * 0.034 / 2;

  // Print the distance or indicate out of range
  if (distanceCm == 0 || distanceCm > 1050) {
    Serial.println("Out of Range");
  } else {
    Serial.print("Distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");
  }

  delay(500);
}

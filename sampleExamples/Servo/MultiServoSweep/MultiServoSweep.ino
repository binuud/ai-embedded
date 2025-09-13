#include <ESP32Servo.h>

// Number of servos
const int servoCount = 2;

// Define servo pins
const int servoPins[servoCount] = {17, 15};
int totalCount = 5;
int count = 0;
// Create Servo objects
Servo servos[servoCount];

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Multiple Servo Control");

  // Attach each servo to its pin
  for (int i = 0; i < servoCount; i++) {
    if (!servos[i].attach(servoPins[i])) {
      Serial.print("Failed to attach servo at pin ");
      Serial.println(servoPins[i]);
    }
  }
}

void loop() {

  count = count + 1;
  if (count > totalCount) {
    delay(2000);
    return;
  }
  // Move all servos from 0 to 180 degrees and back
  for (int angle = 0; angle <= 180; angle++) {
    for (int i = 0; i < servoCount; i++) {
      servos[i].write(angle);
    }
    delay(20);
  }

  for (int angle = 180; angle >= 0; angle--) {
    for (int i = 0; i < servoCount; i++) {
      servos[i].write(angle);
    }
    delay(20);
  }
}

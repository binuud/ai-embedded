#include <ESP32Servo.h>

Servo myservo;          // create servo object to control a servo
int servoPin = 18;       // pin where the servo is connected

int angles[] = {0, 45, 90, 135, 180, 0}; // array of angles to move the servo
int totalCount = sizeof(angles) / sizeof(angles[0]);
int count = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Servo Control with Angle Array");

  if (!myservo.attach(servoPin,500 , 2500)) {
    Serial.print("Failed to attach servo at pin ");
    Serial.println(servoPin);
  }
}

void loop() {
  if (count >= totalCount) {
    myservo.write(0);
    Serial.printf("Moving to 0 start degrees\n");
    delay(10000);  // pause before repeating
    count = 0;    // reset for next loop
  }

  int angle = angles[count];
  Serial.printf("Moving to %d degrees\n", angle);
  myservo.write(angle);
  delay(2000); // wait for servo to reach position

  count++;
}

#include <Stepper.h>

const int stepsPerRevolution = 2048;  // steps per revolution for 28BYJ-48

// Define motor driver input pins connected to ESP32 GPIOs
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

// Initialize Stepper library. The order of pins for 28BYJ-48: IN1, IN3, IN2, IN4
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  myStepper.setSpeed(10);  // motor speed in RPM
  Serial.println("Stepper Motor Control with ESP32");
}

void loop() {

  myStepper.setSpeed(10);  // motor speed in RPM
  Serial.println("Slow Rotating clockwise...");
  myStepper.step(stepsPerRevolution);  // one revolution clockwise
  delay(1000);

  Serial.println("Rotating counterclockwise...");
  myStepper.step(-stepsPerRevolution);  // one revolution counterclockwise
  delay(1000);

  myStepper.setSpeed(30);  // motor speed in RPM

  Serial.println("Faster Rotating clockwise...");
  myStepper.step(stepsPerRevolution);  // one revolution clockwise
  delay(1000);

  Serial.println("Rotating counterclockwise...");
  myStepper.step(-stepsPerRevolution);  // one revolution counterclockwise
  delay(1000);

}

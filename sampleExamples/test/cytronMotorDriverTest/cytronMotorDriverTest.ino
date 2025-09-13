
 #include "CytronMotorDriver.h"

#define LEFT_FORWARD_PIN    19  // Built-in LED is usually at GPIO2
#define LEFT_BACKWARD_PIN   18  // Built-in LED is usually at GPIO2
#define RIGHT_FORWARD_PIN    5  // Built-in LED is usually at GPIO2
#define RIGHT_BACKWARD_PIN   17  // Built-in LED is usually at GPIO2

// Configure the motor driver.
CytronMD motor1(PWM_PWM, LEFT_FORWARD_PIN, LEFT_BACKWARD_PIN);   // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD motor2(PWM_PWM, RIGHT_FORWARD_PIN, RIGHT_BACKWARD_PIN); // PWM 2A = Pin 10, PWM 2B = Pin 11.


// The setup routine runs once when you press reset.
void setup() {
  
}


// The loop routine runs over and over again forever.
void loop() {
  motor1.setSpeed(128);   // Motor 1 runs forward at 50% speed.
  motor2.setSpeed(-128);  // Motor 2 runs backward at 50% speed.
  delay(1000);
  
  motor1.setSpeed(255);   // Motor 1 runs forward at full speed.
  motor2.setSpeed(-255);  // Motor 2 runs backward at full speed.
  delay(1000);

  motor1.setSpeed(0);     // Motor 1 stops.
  motor2.setSpeed(0);     // Motor 2 stops.
  delay(1000);

  motor1.setSpeed(-128);  // Motor 1 runs backward at 50% speed.
  motor2.setSpeed(128);   // Motor 2 runs forward at 50% speed.
  delay(1000);
  
  motor1.setSpeed(-255);  // Motor 1 runs backward at full speed.
  motor2.setSpeed(255);   // Motor 2 runs forward at full speed.
  delay(1000);

  motor1.setSpeed(0);     // Motor 1 stops.
  motor2.setSpeed(0);     // Motor 2 stops.
  delay(1000);
}

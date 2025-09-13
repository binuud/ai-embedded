#include <ESP32Servo.h>
#include <HardwareSerial.h>
#include <CytronMotorDriver.h>
#include <iotCmd.h>

#define LEFT_FORWARD_PIN    19  // Built-in LED is usually at GPIO2
#define LEFT_BACKWARD_PIN   18  // Built-in LED is usually at GPIO2
#define RIGHT_FORWARD_PIN    5  // Built-in LED is usually at GPIO2
#define RIGHT_BACKWARD_PIN   17  // Built-in LED is usually at GPIO2

#define RGB_PIN 2  // Built-in LED is usually at GPIO2

// Configure the motor driver.
CytronMD leftMotor(PWM_PWM, LEFT_FORWARD_PIN, LEFT_BACKWARD_PIN);   // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD rightMotor(PWM_PWM, RIGHT_FORWARD_PIN, RIGHT_BACKWARD_PIN); // PWM 2A = Pin 10, PWM 2B = Pin 11.

// when we started, we had a different UI command flow to trigger speed. Now speed and movement is via same IotCmd object
// will be deprecated
int g_speed = 125; 
int g_max_pwm = 250;

// Number of servos
const int servoCount = 3;

// Define servo pins
const int servoPins[servoCount] = {27, 32, 33}; // servo pins

// Create Servo objects
Servo servos[servoCount];

// Alter the state of the onboard led
// To be used when there is a command sent on bluetooth
void alterInBuiltLed(int state) {
#ifdef RGB_PIN
  digitalWrite(RGB_PIN, state);
#endif
}

void configPins() {
    
  pinMode(RGB_PIN, OUTPUT);  // Initialize the LED pin as output

  pinMode(LEFT_FORWARD_PIN, OUTPUT); 
  pinMode(LEFT_BACKWARD_PIN, OUTPUT); 
  pinMode(RIGHT_FORWARD_PIN, OUTPUT); 
  pinMode(RIGHT_BACKWARD_PIN, OUTPUT); 

    // Attach each servo to its pin
  for (int i = 0; i < servoCount; i++) {
    if (!servos[i].attach(servoPins[i])) {
      Serial.printf("Failed to attach servo %d at pin %d \n", i, servoPins[i]);
    } else {
      Serial.printf("Servo %d up on pin %d \n", i, servoPins[i]);
    }
  }

}

void setSpeed(int speed) {
  if (speed >= 0 && speed <= 100) {
    g_speed = (speed / 100.0) * g_max_pwm;
    Serial.printf("\n Setting speed %d\n", g_speed);
  } else {
    Serial.printf("\n Speed not in limit 0 -- 100 %d", speed);
  }
}

void moveForward() {
  alterInBuiltLed(HIGH);
  leftMotor.setSpeed(g_speed);
  rightMotor.setSpeed(g_speed);
  Serial.println("********** Move Forward");
}

void moveBackward() {
  // alterInBuiltLed(HIGH);
  leftMotor.setSpeed(-g_speed);
  rightMotor.setSpeed(-g_speed);
  Serial.println("********** Move Backward");
}

void turnLeft() {
  alterInBuiltLed(HIGH);
  leftMotor.setSpeed(-g_speed);
  rightMotor.setSpeed(g_speed);
  Serial.println("********** Turn Left");
}

void turnRight() {
  alterInBuiltLed(HIGH);
  leftMotor.setSpeed(g_speed);
  rightMotor.setSpeed(-g_speed);
  Serial.println("********** Turn Right");
}

void carStop() {
  setSpeed(0);
  alterInBuiltLed(LOW);
  leftMotor.setSpeed(LOW);
  rightMotor.setSpeed(LOW);
  Serial.println("********** Car Stop");
}

void controlpadWithSpeed(IotCommand* cmd) {
  int speedInt =  cmd->value1;
  setSpeed(speedInt);
  if (cmd->subcmd == SubCmdEnum_move_forward) moveForward();
  else if (cmd->subcmd == SubCmdEnum_move_backward) moveBackward();
  else if (cmd->subcmd == SubCmdEnum_move_turn_left) turnLeft();
  else if (cmd->subcmd == SubCmdEnum_move_turn_right) turnRight();
  else if (cmd->subcmd == SubCmdEnum_move_stop) carStop();
}


/// @brief Changes angle on the chosen servo
/// @param servoNumber 
/// @param angle 
void controlServo(int servoNumber, int angle) {
  // servo number index starts in arduino at 0, client index starts at 1
  servoNumber--;
  Serial.printf("Setting Servo %d, Angle %d \n", servoNumber, angle);
  servos[servoNumber].write(angle);
}

void controlLed(int ledPin, int value) {

  Serial.printf("Setting Led Pin %d, value %d \n", ledPin, value);
  digitalWrite(ledPin, value);
}
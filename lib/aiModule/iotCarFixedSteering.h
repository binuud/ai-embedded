#ifndef IOT_CAR_H
#define IOT_CAR_H

// only one of iotCarFluidSteering or iotCarFixedSteering can be used

#include <HardwareSerial.h>
#include <CytronMotorDriver.h>
#include <iotCmd.h>
#include <boardState.h>

#define RGB_PIN 2  // Built-in LED is usually at GPIO2

// function headers
void configCar(int motorA1, int  motorA2, int motorB1, int  motorB2);

#define ROBOT_WIDTH 10
#define WHEEL_DIAMETER 2.5
// Car motors
CytronMD* leftMotor = nullptr;
CytronMD* rightMotor = nullptr;

// when we started, we had a different UI command flow to trigger speed. Now speed and movement is via same IotCmd object
// will be deprecated
int g_speed = 125; 
int g_max_pwm = 250;


// Alter the state of the onboard led
// To be used when there is a command sent on bluetooth
void alterInBuiltLed(int state) {
#ifdef RGB_PIN
  digitalWrite(RGB_PIN, state);
#endif
}

void initializeIODevices(ConnectedIO devices[]) {

  int servoCount = 1;
  // iterate through all the devices
  for(size_t i=0; i<MAX_ITEMS; i++) {
    ConnectedIO io = devices[i];
    if (io.dev == DeviceCategory_led) {
      pinMode(io.pin[0], OUTPUT);  // Initialize the LED pin as output
    } else if (io.dev == DeviceCategory_car_2_wheel_module) {
      configCar(io.pin[0], io.pin[1], io.pin[2], io.pin[3]);
    }
  }
    
  pinMode(RGB_PIN, OUTPUT);  // Initialize the LED pin as output



  // TODO BINU, make this dynamic, later
  // nema setup

}

void setSpeed(int speed) {
  if (speed >= 0 && speed <= 100) {
    g_speed = (speed / 100.0) * g_max_pwm;
    Serial.printf("\n Setting speed %d\n", g_speed);
  } else {
    Serial.printf("\n Speed not in limit 0 -- 100 %d", speed);
  }
}

void moveWheels(int vl, int vr) {
  alterInBuiltLed(HIGH);
  leftMotor->setSpeed(vl);
  rightMotor->setSpeed(vr);
  Serial.println("********** Move with vector");
}

void moveForward() {
  alterInBuiltLed(HIGH);
  leftMotor->setSpeed(g_speed);
  rightMotor->setSpeed(g_speed);
  Serial.println("********** Move Forward");
}

void moveBackward() {
  // alterInBuiltLed(HIGH);
  leftMotor->setSpeed(-g_speed);
  rightMotor->setSpeed(-g_speed);
  Serial.println("********** Move Backward");
}

void turnLeft() {
  alterInBuiltLed(HIGH);
  leftMotor->setSpeed(-g_speed);
  rightMotor->setSpeed(g_speed);
  Serial.println("********** Turn Left");
}

void turnRight() {
  alterInBuiltLed(HIGH);
  leftMotor->setSpeed(g_speed);
  rightMotor->setSpeed(-g_speed);
  Serial.println("********** Turn Right");
}

void carStop() {
  setSpeed(0);
  alterInBuiltLed(LOW);
  leftMotor->setSpeed(LOW);
  rightMotor->setSpeed(LOW);
  Serial.println("********** Car Stop");
}

void move(int velocity, int turnStrength) {
  alterInBuiltLed(HIGH);
  if (velocity == 0 && turnStrength == 0) {
    carStop();
  } else if (turnStrength < 10  && turnStrength > -10) {
    if (velocity > 0) {
      setSpeed(velocity);
      moveForward();
    } else if (velocity < 0) {
      setSpeed(-velocity);
      moveBackward();
    }
  } else if (turnStrength > 10) {
      setSpeed(turnStrength);
      turnRight();
  } else if (turnStrength < -10) {
      setSpeed(-turnStrength);
      turnLeft();
  }

  Serial.println("********** Move with speed, turn value");
}

void moveWithVector(int speed, int angle) {
  alterInBuiltLed(HIGH);
  if (speed == 0 && angle == 0) {
    carStop();
  } else {
    // Calculate left and right wheel speeds using inverse kinematic equations
    float v_left = (speed - angle * ROBOT_WIDTH) / (WHEEL_DIAMETER/2);//(rad/s)
    // v_left= v_left*(60/(2*M_PI));//(RPM)
    float v_right =(speed + angle * ROBOT_WIDTH) / (WHEEL_DIAMETER/2);//(rad/s)
    // v_right= v_right*(60/(2*M_PI));//(RPM)
    Serial.printf("Calculated vl(%3.5f) vr(%3.5f)", v_left, v_right);
    moveWheels((int)v_left, (int)v_right);
  }

  Serial.println("********** Move with speed, turn value");
}

void controlpadWithSpeed(IotCommand* cmd) {
  int speedInt =  cmd->value1;
  int turnValue  =  cmd->value2;
  if (cmd->subcmd == SubCmdEnum_vec_move) { moveWithVector(speedInt, turnValue); }
  else if (cmd->subcmd == SubCmdEnum_move) { move(speedInt, turnValue); }
  else if (cmd->subcmd == SubCmdEnum_move_forward) { setSpeed(speedInt);moveForward();}
  else if (cmd->subcmd == SubCmdEnum_move_backward) {setSpeed(speedInt);moveBackward();}
  else if (cmd->subcmd == SubCmdEnum_move_turn_left) {setSpeed(speedInt);turnLeft();}
  else if (cmd->subcmd == SubCmdEnum_move_turn_right) {setSpeed(speedInt);turnRight();}
  else if (cmd->subcmd == SubCmdEnum_move_stop) carStop();
}

//------------ CAR ---------------------
void configCar(int motorA1, int  motorA2, int motorB1, int  motorB2) {
  // pinMode(motorA1, OUTPUT); 
  // pinMode(motorA2, OUTPUT); 
  // pinMode(motorB1, OUTPUT); 
  // pinMode(motorB2, OUTPUT);
  
  // Configure the motor driver.
  leftMotor = new CytronMD(PWM_PWM, motorA1, motorA2);  
  rightMotor = new CytronMD(PWM_PWM, motorB1, motorB2); 
  
  Serial.printf("Left motor pins %d, %d \n", motorA1, motorA2);
  Serial.printf("Right motor pins %d, %d \n", motorB1, motorB2);
  
}


void controlLed(int ledPin, int value) {
  Serial.printf("Setting Led Pin %d, value %d \n", ledPin, value);
  digitalWrite(ledPin, value);
}

#endif // IOT_CAR_H
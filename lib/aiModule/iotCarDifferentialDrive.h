#ifndef IOT_CAR_H
#define IOT_CAR_H

// only one of iotCarFluidSteering or iotCarFixedSteering can be used

#include <HardwareSerial.h>
#include <CytronMotorDriver.h>
#include <iotCmd.h>
#include <boardState.h>

// Load BMM150 headers, if compass is enabled
#if COMPASS_ENABLED

// If not using a developer board, remember to use pullup resistor
const int SDA_PIN =  22; // 8; // 44; //  13;
const int SCL_PIN =  21; //9; //43; // 14;

#include <Wire.h>
#include <DFRobot_BMM150.h>

DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);

#endif

#define RGB_PIN 2  // Built-in LED is usually at GPIO2

void initializeIODevices(ConnectedIO devices[]);
void setSpeed(int speed);
float getCompassReading();
void executeTurn(float speed);
void turnToAngle();

// function headers
void configCar(int motorA1, int  motorA2, int motorB1, int  motorB2);

#define ROBOT_WIDTH 10
#define WHEEL_DIAMETER 2.5
// Car motors
CytronMD* leftMotor = nullptr;
CytronMD* rightMotor = nullptr;

// car lock to angle is only enabled when this is set to true
// else the car vector command will not work
// to set car angle alone, set velocity to 0, and angle to the corresponding angle the car has to turn
bool ANGLE_PID = false;

// when we started, we had a different UI command flow to trigger speed. Now speed and movement is via same IotCmd object
// will be deprecated
int g_speed = 125; 
int g_max_pwm = 250;

// PID Tuning Parameters
float Kp = 4.5;   // Proportional Gain (Start here)
float Ki = 0.05;  // Integral Gain (Handles steady-state error)
float Kd = 0.2;   // Derivative Gain (Dampens overshoot)

float targetAngle = 0.0; // Desired absolute turn angle in degrees
float currentAngle = 0.0;
float errorSum = 0.0;
float lastError = 0.0;
unsigned long lastTime = 0;

// Deadband / Threshold Constraints
const float ANGLE_TOLERANCE = 3.0;   // Stop within 2 degree BMM150 3 degree is safe, 1 degree is too much jitter
const int MIN_MOTOR_SPEED   = 60;    // Minimum PWM to overcome friction
const int MAX_MOTOR_SPEED   = 200;   // Cap maximum turn speed

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
    // Serial.printf("\n Setting speed %d\n", g_speed);
  } else {
    // Serial.printf("\n Speed not in limit 0 -- 100 %d", speed);
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
  // Serial.println("********** Car Stop");
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

void setCarAngle(float angle) {
  ANGLE_PID = true;
  targetAngle = angle;
  Serial.printf("Setting target angle %.2f, PID bool %s \n ", angle, (bool)ANGLE_PID ? "true" : "false");
}

void moveWithVector(int speed, int angle) {

  alterInBuiltLed(HIGH);

  if (speed == 0 && angle == 0) {
    carStop();
  } else if (speed == 0 && angle !=0 ) {
    // if only angle is sent
    setCarAngle(angle);
  } else {
    targetAngle += angle;
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

void carLoop() {

#if COMPASS_ENABLED
  // return if angle PID check is false,
  // we only want the car to turn when 
  if (ANGLE_PID == false) return;
  // Compute error normalized between -180 and +180 degrees
  float currentAngle = getCompassReading();
  float error = targetAngle - currentAngle;
  while (error > 180)  error -= 360;
  while (error < -180) error += 360;

  // stop turning, when tolerance reached
  if (abs(error) > ANGLE_TOLERANCE) {
    turnToAngle();
  } else {
    ANGLE_PID = false;
    carStop();
  }
#endif

}



void turnToAngle() {

  // Calculate delta time
  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  if (dt <= 0.0) return;
  lastTime = currentTime;

  // threshold turn, so car does not jitter
  float currentAngle = getCompassReading();
  if (abs(currentAngle) < 0.05) currentAngle = 0; 
  currentAngle += currentAngle * dt;

  // Compute error normalized between -180 and +180 degrees
  float error = targetAngle - currentAngle;
  while (error > 180)  error -= 360;
  while (error < -180) error += 360;

  // // stop turning, when tolerance reached
  if (abs(error) <= ANGLE_TOLERANCE) {
    return;
  }

  // Run PID calculations
  errorSum += error * dt;
  errorSum = constrain(errorSum, -50, 50); // Windup guard
  float errorDeriv = (error - lastError) / dt;
  lastError = error;

  float pidOutput = (Kp * error) + (Ki * errorSum) + (Kd * errorDeriv);

  // 6. Direct motors to pivot in place
  executeTurn(pidOutput);

  // Debug reporting
  Serial.print("Target: "); Serial.print(targetAngle);
  Serial.print(" | Current: "); Serial.print(currentAngle);
  Serial.print(" | Error: "); Serial.println(error);
  
  delay(10); // Maintain fixed frequency cycle

}

void executeTurn(float speed) {
  // Determine direction based on PID output sign
  int rotationDirection = 1;
  if (speed > 0) {
    rotationDirection = -1;
  }

  bool direction = (speed > 0); 
  int absSpeed = abs(speed);

  // Inject minimum power to prevent stalling, limit maximum output
  if (absSpeed > 0) {
    absSpeed = constrain(absSpeed, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  }

  // if it is a positive angle, rotate clock wise, if negative angle, rotate counter clock wise
  moveWheels((int)absSpeed *  rotationDirection, (int)absSpeed * -rotationDirection);

}


void controlLed(int ledPin, int value) {
  Serial.printf("Setting Led Pin %d, value %d \n", ledPin, value);
  digitalWrite(ledPin, value);
}

void initCompass() {
  
#if COMPASS_ENABLED
  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C bus with ESP32 pins
  int count = 0;
  while(bmm150.begin()) { // Initialize sensor, retry if failed
    Serial.printf("bmm150 init failed, Check pullup resistor, connect SDA,SCL to pin (%d,%d)\n",SDA_PIN, SCL_PIN);
    if (count++ > 4) {
      Serial.println("bmm150 setup failed!");
      return;
    }
    delay(2000);
  }

  bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);      // Normal mode
  bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY); // High accuracy preset
  bmm150.setRate(BMM150_DATA_RATE_10HZ);                 // Set data rate to 10Hz
  bmm150.setMeasurementXYZ();  
  Serial.println("bmm150 setup success!");

#endif

}


float getCompassReading() {
  #if COMPASS_ENABLED
  // sBmm150MagData_t magData = bmm150.getGeomagneticData(); // Read magnetic data
    return(bmm150.getCompassDegree()); 
  #else
    return(0.0); 
  #endif
}


void initCar() {

#if COMPASS_ENABLED
  initCompass();
#endif

}

#endif // IOT_CAR_H
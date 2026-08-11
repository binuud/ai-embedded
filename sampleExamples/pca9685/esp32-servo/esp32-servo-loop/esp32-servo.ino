/*
Checking if PCA9685 can be used to control servo motors drivers
Working, as on 11 Aug 2026
Simple looping program, that changes angles of 2 servos, connected at position 7 and 12 on PCA9685
*/
 
// Include Wire Library for I2C
#include <Wire.h>
 
// Include Adafruit PCA9685 Servo Library
#include <Adafruit_PWMServoDriver.h>
 
// Creat object to represent PCA9685 at default I2C address
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);
 
// Define maximum and minimum number of "ticks" for the servo motors
// Range from 0 to 255, PCA9685 supports in range  0 to 4095
// This determines the pulse width
 
#define SERVOMIN  125  // Minimum value
#define SERVOMAX  575  // Maximum value

// Define servo motor connections (expand as required)
#define SER0  7   //Servo Motor 0 on connector 7
#define SER1  11  //Servo Motor 1 on connector 11

#define I2C_SDA 21
#define I2C_SCL 22


// Variables for Servo Motor positions (expand as required)
int pwm0;
int pwm1;
 
void setup() {
 
  // Serial monitor setup
  Serial.begin(115200);
 
  // Print to monitor
  Serial.println("PCA9685 Servo Test");

  // Initialize I2C with custom SDA and SCL pins
  if (Wire.begin(I2C_SDA, I2C_SCL)) {
    Serial.println("I2C initialized successfully.");
  } else {
    Serial.println("I2C initialization failed!");
    while (true); // Stop execution if I2C fails
  }
 
  
  // Initialize PCA9685
  pca9685.begin();
 
  // Set PWM Frequency to 50Hz
  pca9685.setPWMFreq(60);
 
}
 
void loop() {
  Serial.println("0 to 180");
  // Move Motor 0 from 0 to 180 degrees
 for( int angle =0; angle<181; angle +=10){
    pca9685.setPWM(SER0, 0, angleToPulse(angle) );
    pca9685.setPWM(SER1, 0, angleToPulse(180-angle) ); 
    delay(100); 
  }
  delay(1000);
  for( int angle =180; angle>=0; angle -=10){
    pca9685.setPWM(SER0, 0, angleToPulse(angle) );
    delay(100); 
  }
  delay(1000);
  for( int angle =180; angle>=0; angle -=10){
    pca9685.setPWM(SER1, 0, angleToPulse(angle) );
    delay(100); 
  }

  // robojax PCA9865 16 channel Servo control
  delay(1000);
  // Serial.println("180 to 0");
  for( int angle =0; angle<181; angle +=10){
    pca9685.setPWM(SER0, 0, angleToPulse(angle) );
    pca9685.setPWM(SER1, 0, angleToPulse(180-angle) ); 
    delay(100); 
  }
  delay(1000);
 
 
}

/*
 * angleToPulse(int ang)
 * gets angle in degree and returns the pulse width
 * also prints the value on seial monitor
 * written by Ahmad Shamshiri for Robojax, Robojax.com
 */
int angleToPulse(int ang){
   int pulse = map(ang,0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}
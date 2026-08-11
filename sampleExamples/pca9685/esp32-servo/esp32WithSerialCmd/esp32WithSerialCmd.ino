/*
Checking if PCA9685 can be used to control servo motors drivers
Allows servo to be controlled by serial input,
Allows both servo and motor driver to be controlled by PCA9685
Working as on 11,Aug,2026
send pulse at 4000 for max speed, pulse 0 to stop
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


#define I2C_SDA 21
#define I2C_SCL 22


// Variables for Servo Motor positions (expand as required)
int pwm0;
int pwm1;

int splitString(const String& input, char delimiter, String output[], int maxParts) {
    int partIndex = 0;
    int startIndex = 0;
    int delimIndex;

    while (partIndex < maxParts - 1) {
        delimIndex = input.indexOf(delimiter, startIndex);
        if (delimIndex == -1) break;

        output[partIndex] = input.substring(startIndex, delimIndex);
        startIndex = delimIndex + 1;
        partIndex++;
    }
    output[partIndex] = input.substring(startIndex); // last part
    return partIndex + 1; // total parts found
}

void help() {

  Serial.printf("Control servo with serial commands. Pinout (SDA, SCL)(%d, %d)", I2C_SDA, I2C_SCL);
  Serial.println(" h -- help message");
  Serial.println(" s -- control servo, s[SERVO_NUM],[ANGLE]");
  Serial.println(" eg: s1,100 (To Move servo at pin 1 to angle 100)");
  Serial.println(" eg: s12,180 (To Move servo at pin 12 to angle 180)");
  Serial.println(" m -- control motor, m[MOTOR_NUM],[SPEED]");
  Serial.println(" eg: s1,2000 (To Move motor at pin 1 at speed 2000)");
  Serial.println("OK");
  Serial.println("----------------------");

}

// serialHandler
// checks if input in h, then displays help
// if input is in format c,n,angle eg: c,1,100, it moves servo 1 to angle 100
void serialHandler() {

  if (Serial.available() > 0) {
      
    String input = Serial.readStringUntil('\n');
    input.trim();

    String prefix = input.substring(0, 1);  // Get the first character
    prefix.toUpperCase();   

    if (prefix.startsWith("H")) {
      help();
      return;
    } else if (prefix.startsWith("S") || prefix.startsWith("M")) {
      handleSerialCommand(prefix, input);
      return;
    } else {
      
      Serial.println("ERROR: Commands must start with 'h' or 's' or 'm'");
      help();
      return;
    }

  }

}

void handleSerialCommand(String prefix, String input) {
  String params = input.substring(2);
  params.trim();
  Serial.printf("Input (%s) Params (%s)", input, params);
  if (params.length() > 0) {
    String parts[4]; // adjust size as needed, 4 for future use
    int numParts = splitString(params, ',', parts, 4);
    if (numParts == 2) {
      int motorNum = parts[0].toInt();
      int angle = parts[1].toInt();
      Serial.printf("Input (%s), motor(%d), angle(%d)", input, motorNum, angle);
      if (prefix == "S") {
        pca9685.setPWM(motorNum, 0, angleToPulse(angle) );
      } else if (prefix == "M") {
        pca9685.setPWM(motorNum, 0, angle );
      }
      
    }
  }
}
 
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
  help();
 
}
 
void loop() {
  serialHandler();
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
   Serial.printf("Angle: (%d), pulse (%d)\n", ang, pulse);
   return pulse;
}
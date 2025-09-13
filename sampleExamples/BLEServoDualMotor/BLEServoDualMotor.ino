#include <ESP32Servo.h>
#include "CytronMotorDriver.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID          "6e400001-b5a3-f393-e0a9-e50e24dcca9e" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define RGB_PIN 2  // Built-in LED is usually at GPIO2

#define LEFT_FORWARD_PIN    19  // Built-in LED is usually at GPIO2
#define LEFT_BACKWARD_PIN   18  // Built-in LED is usually at GPIO2
#define RIGHT_FORWARD_PIN    5  // Built-in LED is usually at GPIO2
#define RIGHT_BACKWARD_PIN   17  // Built-in LED is usually at GPIO2


// Number of servos
const int servoCount = 3;

// Define servo pins
const int servoPins[servoCount] = {27, 32, 33}; // servo pins

// Create Servo objects
Servo servos[servoCount];

// Configure the motor driver.
CytronMD leftMotor(PWM_PWM, LEFT_FORWARD_PIN, LEFT_BACKWARD_PIN);   // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD rightMotor(PWM_PWM, RIGHT_FORWARD_PIN, RIGHT_BACKWARD_PIN); // PWM 2A = Pin 10, PWM 2B = Pin 11.


BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// globals
bool g_convert = false;
String g_rxString = "";

int g_speed = 125;
int g_max_pwm = 250;

void alterInBuiltLed(int state) {
#ifdef RGB_PIN
  digitalWrite(RGB_PIN, state);
#endif
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
  alterInBuiltLed(HIGH);
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

void controlpadWithSpeed(String direction, int speed) {
  String cmd = direction.substring(0, 2);
  
  setSpeed(speed);
  if (cmd == "FF") moveForward();
  else if (cmd == "BB") moveBackward();
  else if (cmd == "LL") turnLeft();
  else if (cmd == "RR") turnRight();
  else if (cmd == "SS") carStop();
}

/// @brief Changes angle on the chosen servo
/// @param servoNumber 
/// @param angle 
void controlServo(int servoNumber, int angle) {
  Serial.printf("Setting Servo %d, Angle %d \n", servoNumber, angle);
  servos[servoNumber].write(angle);
}


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 4) {
      String cmd = rxValue.substring(0, 4);
      String val = rxValue.substring(5);
      Serial.printf("Received Value %s=%s \n", cmd, val);
      if (cmd == "SR01") {
        // handle servo
        controlServo(0, val.toInt());
      } else if (cmd == "SR02") {
        controlServo(1, val.toInt());
      } else if (cmd == "SR03") {
        controlServo(2, val.toInt());
      } else {
        // handle robot movement
        controlpadWithSpeed(cmd, val.toInt());
      }
      
    }
  }
};

void setup() {
  
  Serial.begin(115200);

  // Attach each servo to its pin
  for (int i = 0; i < servoCount; i++) {
    if (!servos[i].attach(servoPins[i])) {
      Serial.printf("Failed to attach servo %d at pin %d \n", i, servoPins[i]);
    } else {
      Serial.printf("Servo %d up on pin %d \n", i, servoPins[i]);
    }
  }
  
  pinMode(RGB_PIN, OUTPUT);  // Initialize the LED pin as output

  pinMode(LEFT_FORWARD_PIN, OUTPUT); 
  pinMode(LEFT_BACKWARD_PIN, OUTPUT); 
  pinMode(RIGHT_FORWARD_PIN, OUTPUT); 
  pinMode(RIGHT_BACKWARD_PIN, OUTPUT); 

  // Create the BLE Device
  BLEDevice::init("Brain Robot");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  // Descriptor 2902 is not required when using NimBLE as it is automatically added based on the characteristic properties
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.printf("BLE Service id %s, characteristic %s", SERVICE_UUID, CHARACTERISTIC_UUID_RX);
  Serial.println("Waiting a client connection to notify...");
}




void loop() {
  
  if (deviceConnected) {
    // Serial.print("Notifying Value: ");
    // Serial.println(txValue);
    // pTxCharacteristic->setValue(&txValue, 1);
    // pTxCharacteristic->notify();
    // txValue++;
    // delay(1000);  // Notifying every 1 second
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("Started advertising again...");
    oldDeviceConnected = false;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = true;
  }
}

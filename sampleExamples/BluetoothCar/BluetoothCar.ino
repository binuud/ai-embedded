/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define RGB_PIN 2  // Built-in LED is usually at GPIO2

#define LEFT_FORWARD_PIN    19  // Built-in LED is usually at GPIO2
#define LEFT_BACKWARD_PIN   18  // Built-in LED is usually at GPIO2
#define RIGHT_FORWARD_PIN    5  // Built-in LED is usually at GPIO2
#define RIGHT_BACKWARD_PIN   17  // Built-in LED is usually at GPIO2

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

bool g_convert = false;
String g_rxString = "";



void alterInBuiltLed(int state) {
#ifdef RGB_PIN
  digitalWrite(RGB_PIN, state);
#endif
}

void moveForward() {
  alterInBuiltLed(HIGH);
  digitalWrite(LEFT_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_FORWARD_PIN, HIGH);
  Serial.println("********** Move Forward");
}

void moveBackward() {
  alterInBuiltLed(HIGH);
  digitalWrite(LEFT_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_BACKWARD_PIN, HIGH);
  Serial.println("********** Move Backward");
}

void turnLeft() {
  alterInBuiltLed(HIGH);
  digitalWrite(LEFT_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_FORWARD_PIN, HIGH);
  Serial.println("********** Turn Left");
}

void turnRight() {
  alterInBuiltLed(HIGH);
  digitalWrite(LEFT_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_BACKWARD_PIN, HIGH);
  Serial.println("********** Turn Right");
}

void carStop() {
  alterInBuiltLed(LOW);
  digitalWrite(LEFT_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_FORWARD_PIN, LOW);
  digitalWrite(LEFT_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_BACKWARD_PIN, LOW);
  Serial.println("********** Car Stop");
}

// ************************* CONVERT CONTROLPAD CODE ************************
void controlpad(String rxString) {

  if (rxString == "!B516") moveForward();
  else if (rxString == "!B615") moveBackward();
  else if (rxString == "!B714") turnLeft();
  else if (rxString == "!B813") turnRight();

  else if (rxString == "!B507") carStop(); // Serial.println("********** Stop Action UP");
  else if (rxString == "!B606") carStop(); // Serial.println("********** Stop Action DOWN");
  else if (rxString == "!B705") carStop(); // Serial.println("********** Stop Action LEFT");
  else if (rxString == "!B804") carStop(); // Serial.println("********** Stop Action RIGHT"); 

  else if (rxString == "!B11:") Serial.println("********** Start Action 1");
  else if (rxString == "!B219") Serial.println("********** Start Action 2");
  else if (rxString == "!B318") Serial.println("********** Start Action 3");
  else if (rxString == "!B417") Serial.println("********** Start Action 4");

  else if (rxString == "!B10;") Serial.println("********** Stop Action 1");
  else if (rxString == "!B20:") Serial.println("********** Stop Action 2");
  else if (rxString == "!B309") Serial.println("********** Stop Action 3");
  else if (rxString == "!B408") Serial.println("********** Stop Action 4");

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

    if (rxValue.length() > 0) {
      Serial.printf("Received Value %s \n", rxValue);
      controlpad(rxValue);
    }
  }
};

void setup() {

  
  Serial.begin(115200);

  pinMode(RGB_PIN, OUTPUT);  // Initialize the LED pin as output

  pinMode(LEFT_FORWARD_PIN, OUTPUT); 
  pinMode(LEFT_BACKWARD_PIN, OUTPUT); 
  pinMode(RIGHT_FORWARD_PIN, OUTPUT); 
  pinMode(RIGHT_BACKWARD_PIN, OUTPUT); 

  // Create the BLE Device
  BLEDevice::init("UART Service");

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

#include <ESP32Servo.h>
#include "CytronMotorDriver.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BrainProto.h>

#include "CytronMotorDriver.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID          "6e400001-b5a3-f393-e0a9-e50e24dcca9e" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define RGB_PIN 2  // Built-in LED is usually at GPIO2


// Example usage


BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// Alter the state of the onboard led
// To be used when there is a command sent on bluetooth
void alterInBuiltLed(int state) {
#ifdef RGB_PIN
  digitalWrite(RGB_PIN, state);
#endif
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
    
    uint16_t length = pChar->getValue().length(); // get length
    const uint8_t* data = pChar->getData();       // get raw data pointer

    Serial.print("Received Data (hex): ");
    for (int i = 0; i < length; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    IotCommand cmd = decodeCommand(exampleData);
    debugIotCommand(cmd);

  }
};


void setup() {
  
  Serial.begin(115200);

    // Example data received (10 bytes)
  //  IotCommand myCommand = {Move, Forward, 10, 20, 30};
  uint8_t exampleData[10] = {1, 0, 1, 0, 10, 0, 20, 0, 30, 0};

  IotCommand decodedCmd = decodeCommand(&exampleData);
  debugIotCommand(&decodedCmd);

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

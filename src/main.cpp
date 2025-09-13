#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <iotCmd.h>
#include "iotActuators.h"

#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"


BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;



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
    
    const uint8_t* data = pCharacteristic->getData();       // get raw data pointer

    IotCommand cmd;
    decodeCommand(data, &cmd);
    debugIotCommand(&cmd);
    
    if (cmd.cmd == CmdEnum_servo) {
      // handle servo
      controlServo(cmd.identifier, cmd.value1);
    } else if (cmd.cmd == CmdEnum_move) {
      controlpadWithSpeed(&cmd);
    } else if (cmd.cmd == CmdEnum_led) {
      controlLed(cmd.identifier, cmd.value1);
    }
    
  }
};


void setup() {
  
  Serial.begin(115200);
  Serial.println("Setup Begin...");

  configPins();

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

  Serial.println("Setup Done...");

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

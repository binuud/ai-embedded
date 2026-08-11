
#define CAMERA_ENABLED 0 // if camera is enabled set to 1, else set to 0
#define BLUETOOTH_ENABLED 1 // if bluetooth is enabled
#define WIFI_ENABLED 1 // if bluetooth is enabled
#define CAR_FIXED_STEERING 1 // if 4 motor car is enabled
#define CAR_FLUID_STEERING 0 // if car steering is required (turn and drive motor)
#define CAR_SERVO 0 // if servo moto is needed
#define ACTUATORS 0 // if servo, stepper are defined

#include <HardwareSerial.h>
#include <iotCmd.h>
#include <atCommands/atCommonCommands.h>
#include <serialHandler.h>

#if ACTUATORS
#include <iotActuators.h>
#endif

#if CAR_FIXED_STEERING
#include <atCommands/atCarCommands.h>
#include <iotCarFixedSteering.h>
#endif

#if CAR_FLUID_STEERING
#include <atCommands/atCarCommands.h>
#include <iotCarFluidSteering.h>
#endif

#if BLUETOOTH_ENABLED 
#include <atCommands/atBleCommands.h>
#include <bleConfig.h> 
#endif

#include <serialHandler.h>

// #include "carWithCameraCommands.h"

#if WIFI_ENABLED
#include <wifiInit.h>
#include <atCommands/atWifiCommands.h>
#include <asyncWebServer.h>
#endif

#if CAMERA_ENABLED 
#include <cameraInit.h>
// #include <blockingCameraServer.h>
#include <asyncCamera.h>
#endif




// Binu Udayakumar binu@dronasys.com
// UI tools can be accessed at https://binuud.com

SerialHandler serialHandler(10);

void setup() {
  
  Serial.begin(115200);
  Serial.println("Setup Begin...");
  delay(1000); // wait for serial monitor initialization

  Serial.println("Registering AT Commands for Common items...");
  registerCommonATCommands(serialHandler);

  // load preferences from EEPROM
  // this has to be called first before any other initiations
  // since we are storing pinout information here
  Serial.println("Loading Prefs...");
  DeviceConfig config = devicePrefs.loadConfig();

  // now initialize IO devices with the device information loaded from above.
  Serial.println("Initializing Devices...");
  initializeIODevices(devicePrefs.devices);

#if ACTUATORS
  registerActuatorATCommands(serialHandler);
  initializeActuatorIODevices();
#endif

#if CAR_FLUID_STEERING || CAR_FIXED_STEERING
  registerCarATCommands(serialHandler);
#endif

// init wifi
#if WIFI_ENABLED  
  Serial.println("Initializing wifi...");
  registerWifiATCommands(serialHandler);
  initWifi(devicePrefs.config.wifi_ssid, devicePrefs.config.wifi_password);
#endif

#if BLUETOOTH_ENABLED    
  Serial.println("Setting Bluetooth...");
  registerBleATCommands(serialHandler);
  setupBle();
#endif



  // if CAMERA is enabled, init the camera
#if CAMERA_ENABLED 
  Serial.println("Setting Camera...");
  initCamera();
  initAsyncServer();
#endif

#if WIFI_ENABLED  
  initAsyncServer();
#endif

  serialHandler.help(); // print al the AT-Commands
  Serial.println("Setup Done...");
  Serial.println("Listening for serial Input...");

}


// each module will have to implement its own loop.
// this loop will act like a scheduler without priority
// avoid calls to delay, from with module loops
void loop() {

  // listen for commands from serial interface 
  // and invoke the same
  serialHandler.loop();

  // run ble loop()
#if BLUETOOTH_ENABLED   
  loopBle();
#endif  

#if ACTUATORS
  // run stepper, motors and servo
  loopActuator();
#endif  

}

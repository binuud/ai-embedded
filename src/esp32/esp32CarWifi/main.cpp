// if bluetooth is enabled
#define BLUETOOTH_ENABLED 1 

// if wifi is enabled
#define WIFI_ENABLED 1 

// if 4 or 2 differential drive motor car is enabled, steering by rotating wheels in Clock wise, anti clock wise.
#define CAR_DIFFERENTIAL_DRIVE 1 

#define CAR_FLUID_STEERING 0 // if car steering is required (turn and drive motor)
#define CAR_SERVO 0 // if servo moto is needed
#define ACTUATORS 0 // if servo, stepper are defined
#define COMPASS_ENABLED 0 // if compass, BMM 150 is enabled

#include <HardwareSerial.h>
#include <iotCmd.h>
#include <atCommands/atCommonCommands.h>
#include <serialHandler.h>

#if ACTUATORS
#include <iotActuators.h>
#endif

#if CAR_DIFFERENTIAL_DRIVE
#include <atCommands/atCarCommands.h>
#include <iotCarDifferentialDrive.h>
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

#if CAR_FLUID_STEERING || CAR_DIFFERENTIAL_DRIVE
  initCar();
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


#if WIFI_ENABLED  
  if (WiFi.status() == WL_CONNECTED)  {
    Serial.println("Setting initAsyncServer...");   
    initAsyncServer();
  } else {
    Serial.println("Cannot start AsyncServer, wifi not working");  
  }

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


  // move car to requested position and angle
  carLoop();

}

#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>

#if ACTUATORS
#include <iotActuators.h>
#endif

// add remove AT Commands here

// Forward declarations of command callbacks


void atCmdWifiSSID(const String& args);
void atCmdWifiPass(const String& args);

void atCmdCarConfig(const String& args);
void atCmdCarMove(const String& args);

#if ACTUATORS
void atCmdStepperConfig(const String& args);
void atCmdServoConfig(const String& args);

void atCmdStepperAngle(const String& args);
void atCmdServoAngle(const String& args);

void atReadStepperConfig();
void atReadServoConfig();

void atReadStepperAngle();
void atReadServoAngle();

#endif

void atCmdLedConfig(const String& args);
void atCmdConfig(const String& args);

void atCmdClearStorage(const String& args);

void atReadBleName();

void atReadWifiSSID();
void atReadWifiPass();

void atReadCarConfig();
void atReadCarMove();

void atReadLedConfig();
void atReadConfig();



// Preset array of commands and their callbacks
// link AT Command to the corresponding callbacks here

AtCommand atCommands[] = {

    // this is prefix match, commands coming first, will take precendence in prefix match
    // {"BLE_NAME", "Config BLE Name, eg: AT+BLE_NAME=[BLE-NAME]", atCmdBleName, atReadBleName},
    // {"BLE_PASS", "Config BLE Pass, eg: AT+BLE_PASS=[BLE-PASSWORD]", atCmdBlePass, atReadBlePass},

    {"WIFI_SSID", "Config Wifi SSID, eg: AT+WIFI_SSID=[WIFI-SSID]", atCmdWifiSSID, atReadWifiSSID},
    {"WIFI_PASS", "Config Wifi SSID, eg: AT+WIFI_PASS=[WIFI-PASS]", atCmdWifiPass, atReadWifiPass},

    {"CAR", "Config Car control Pins, eg: AT+CAR=[MotorLeft+], [MotorLeft-], [MotorRight+], [MotorRight-]" , atCmdCarConfig, atReadCarConfig},

#if ACTUATORS    
    {"STEPPER", "Config Stepper Pin, AT+STEPPER=[STEPPER NUMBER], [STEPPER_PIN, DIR_PIN]" , atCmdStepperConfig, atReadStepperConfig},
    {"SERVO", "Config Stepper Pin, AT+SERVO=[STEPPER NUMBER], [STEPPER_PIN, DIR_PIN]" , atCmdServoConfig, atReadServoConfig},
    {"STP_ANGLE", "Move stepper 1 to angle N, AT+STP_ANGLE=1,[Angle]", atCmdStepperAngle, atReadStepperAngle},
    {"SRV_ANGLE", "Move servo 1 to angle N, AT+SRV_ANGLE=1,[Angle]", atCmdServoAngle, atReadServoAngle},
#endif

    {"LED", "Config Led Pin, AT+LED=[LED NUMBER], [LED_PIN]" , atCmdLedConfig, atReadLedConfig},
    {"MOVE", "Move car, AT+MOVE=[W (Forward),A(Left),S(Back),D(Right),Z(Stop)], SPEED", atCmdCarMove, atReadCarMove},

    {"CLEAR_STORAGE", "Clear all stored values", atCmdClearStorage, nullptr}

};


// AT command handlers



// void atCmdBleName(const String& params) {
//     if (params.length() > 0) {
//         devicePrefs.config.ble_name = params;
//         devicePrefs.saveBle();
//         Serial.printf("atCmdBleName new name %s , OK\n", params);
//     } else {
//         Serial.println("ERROR: BLE name required");
//     }
// }

void atCmdWifiSSID(const String& params) {
    if (params.length() > 0) {
        devicePrefs.config.wifi_ssid = params;
        devicePrefs.saveWifi();
        Serial.printf("atCmdWifiSSID new ssid  %s , OK\n", params);
    } else {
        Serial.println("ERROR: SSID required");
    }
}

void atCmdWifiPass(const String& params) {
    if (params.length() > 0) {
        devicePrefs.config.wifi_password = params;
        devicePrefs.saveWifi();
        Serial.printf("atCmdWifiPass new password %s %s , OK\n", params);
    } else {
        Serial.println("ERROR: Password required");
    }
}


void atCmdCarConfig(const String& params) {
    if (params.length() > 0) {
        String parts[4]; // adjust size as needed
        int numParts = splitString(params, ',', parts, 4);
        if (numParts == 4) {
            
            int motorA1 = parts[0].toInt();
            int motorA2 = parts[1].toInt();
            int motorB1 = parts[2].toInt();
            int motorB2 = parts[3].toInt();
            
            String carName = "CAR1" ;
            ConnectedIO io = {DeviceCategory_car_2_wheel_module, carName, {motorA1, motorA2, motorB1, motorB2}, 4};
            devicePrefs.saveIODevice(io);
            devicePrefs.printDevice(io);
            configCar(motorA1, motorA2, motorB1, motorB2);
        } else {
            Serial.println("ERROR: Invalid format");
        }
    } else {
        Serial.println("ERROR: invalid params, format motorA1, motorA2, motorB1, motorB2");
    }
}


void atCmdConfig(const String& params) {
    if (params.length() > 0) {
        Serial.println("atCmdConfig OK");
    } else {
        Serial.println("ERROR: Config parameters required");
    }
}

void atCmdLedConfig(const String& params) {
    if (params.length() > 0) {
        int ledPin = params.toInt();
        String stepperName = "LED1";
        ConnectedIO io = {DeviceCategory_led, stepperName, {ledPin}, 1};
        devicePrefs.saveIODevice(io);
        devicePrefs.printDevice(io);
    } else {
        Serial.println("ERROR: invalid params, format stepper, angle");
    }
}


void atCmdCarMove(const String& params) {
    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        int numParts = splitString(params, ',', parts, 2);
        if (numParts == 2) {
            String action = parts[0];
            action.toUpperCase();   
            int speed = parts[1].toInt();
            if (speed > 100) speed = 100;
            if (speed < 0) speed = 0;
            setSpeed(speed);
            Serial.printf("Car command %s, %d \n", action, speed);
            if (action == "W") moveForward(); 
            else if (action == "S") moveBackward();
            else if (action == "A") turnLeft();
            else if (action == "D") turnRight();
            else if (action == "Z") carStop();
        } else {
            Serial.println("ERROR: Invalid format");
        }
    } else {
        Serial.println("ERROR: invalid params, format Direction[WASDZ], speed");
    }    
}


void atCmdClearStorage(const String& params) {
    devicePrefs.clear();
    Serial.println("Cleared storage");
}


// void atReadBleName() {
//     Serial.printf("Ble name : %s \n", devicePrefs.config.ble_name);    
// }

void atReadWifiSSID() {
    Serial.printf("Wifi ssid : %s \n", devicePrefs.config.wifi_ssid);    
}


void atReadWifiPass() {
    Serial.printf("Wifi Password : %s \n", devicePrefs.config.wifi_password);    
}

void atReadCarConfig() {
    for(int i=0;i<MAX_ITEMS;i++) {
        ConnectedIO io = devicePrefs.devices[i];
        if (io.dev == DeviceCategory_car_2_wheel_module) {
            devicePrefs.printDevice(io);
        }
    }
}


void atReadLedConfig() {
    for(int i=0;i<MAX_ITEMS;i++) {
        ConnectedIO io = devicePrefs.devices[i];
        if (io.dev == DeviceCategory_led) {
            devicePrefs.printDevice(io);
        }
    }
}

void atReadCarMove() {
    Serial.print("Not Implemented : \n");    
}


void atReadConfig() {
    Serial.print("Stepper angle : \n");    
}

#if ACTUATORS

void atCmdAngle(const String& params) {
    int angle = params.toInt();
    if (angle >= 0 && angle <= 360) {
        controlNemaStepper(1, angle);
        Serial.println("OK");
    } else {
        Serial.println("ERROR: Invalid angle (0-360)");
    }
}

void atCmdStepperConfig(const String& params) {

    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        Serial.printf("Configuring stepper start");
        int numParts = splitString(params, ',', parts, 3);
        if (numParts == 3) {
            
            int stepper_id = parts[0].toInt();
            int step_pin = parts[1].toInt();
            int dir_pin = parts[2].toInt();
            String stepperName = "STP" + String(stepper_id);
            ConnectedIO io = {DeviceCategory_stepper, stepperName, {step_pin, dir_pin}, 2};
            devicePrefs.saveIODevice(io);
            devicePrefs.printDevice(io);
            configNemaStepper(step_pin, dir_pin);
        } else {
            Serial.println("ERROR: Invalid format");
        }
    } else {
        Serial.println("ERROR: invalid params, format stepper, gpio");
    }
}

void atCmdServoConfig(const String& params) {

    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        Serial.printf("Configuring servo");
        int numParts = splitString(params, ',', parts, 2);
        if (numParts == 2) {
            
            int servo_id = parts[0].toInt();
            int servo_Pin = parts[1].toInt();
            String servoName = "SRV" + String(servo_id);
            ConnectedIO io = {DeviceCategory_servo, servoName, {servo_Pin}, 1};

            devicePrefs.saveIODevice(io);
            devicePrefs.printDevice(io);
            
            configServo(servo_id, servo_Pin);

        } else {
            Serial.println("ERROR: Invalid format");
        }
    } else {
        Serial.println("ERROR: invalid params, format stepper, gpio");
    }
}

void atCmdStepperAngle(const String& params) {
    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        int numParts = splitString(params, ',', parts, 2);
        if (numParts == 2) {
            int stepper = parts[0].toInt();
            int angle = parts[1].toInt();
            if (angle >= 0 && angle <= 360) {
                controlNemaStepper(stepper, angle);
                Serial.println("OK");
            } else {
                Serial.println("ERROR: Invalid angle (0-360)");
            }
        } else {
            Serial.println("ERROR: Invalid format");
        }
    } else {
        Serial.println("ERROR: invalid params, format stepper, angle");
    }
}

void atCmdServoAngle(const String& params) {
    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        int numParts = splitString(params, ',', parts, 2);
        if (numParts == 2) {
            int servo = parts[0].toInt();
            int angle = parts[1].toInt();
            if (angle >= 0 && angle <= 360) {
                controlServo(servo, angle);
                Serial.println("OK");
            } else {
                Serial.println("ERROR: Invalid angle (0-360)");
            }
        } else {
            Serial.println("ERROR: Invalid format");
        }
    } else {
        Serial.println("ERROR: invalid params, format stepper, angle");
    }
}


void atReadStepperAngle() {
    Serial.println("Stepper angle : ");    
}

void atReadServoAngle() {
    Serial.println("Stepper angle : ");    
}


void atReadServoConfig() {
    for(int i=0;i<MAX_ITEMS;i++) {
        ConnectedIO io = devicePrefs.devices[i];
        if (io.dev == DeviceCategory_servo) {
            devicePrefs.printDevice(io);
        }
    }
}

void atReadStepperConfig() {
    for(int i=0;i<MAX_ITEMS;i++) {
        ConnectedIO io = devicePrefs.devices[i];
        if (io.dev == DeviceCategory_stepper) {
            devicePrefs.printDevice(io);
        }
    }
}

#endif

#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>
#include <iotActuators.h>




void atCmdStepperConfig(const String& args);
void atCmdServoConfig(const String& args);



void atCmdStepperAngle(const String& args);
void atCmdServoAngle(const String& args);


void atReadStepperConfig();
void atReadServoConfig();

void atReadStepperAngle();
void atReadServoAngle();

void registerActuatorATCommands(SerialHandler &cmdManager) {
    
    // Construct individual instances
    AtCommand stepperCmd =   {"STEPPER", "Config Stepper Pin, AT+STEPPER=[STEPPER NUMBER], [STEPPER_PIN, DIR_PIN]" , atCmdStepperConfig, atReadStepperConfig};
    AtCommand servoCmd =  {"SERVO", "Config Stepper Pin, AT+SERVO=[STEPPER NUMBER], [STEPPER_PIN, DIR_PIN]" , atCmdServoConfig, atReadServoConfig};
    AtCommand stepperMove = {"STP_ANGLE", "Move stepper 1 to angle N, AT+STP_ANGLE=1,[Angle]", atCmdStepperAngle, atReadStepperAngle};
    AtCommand servoMove = {"SRV_ANGLE", "Move servo 1 to angle N, AT+SRV_ANGLE=1,[Angle]", atCmdServoAngle, atReadServoAngle};

    // Push them into the manager instance
    cmdManager.add(stepperCmd);
    cmdManager.add(servoCmd);

    cmdManager.add(stepperMove);
    cmdManager.add(servoMove);

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


void atCmdAngle(const String& params) {
    int angle = params.toInt();
    if (angle >= 0 && angle <= 360) {
        controlNemaStepper(1, angle);
        Serial.println("OK");
    } else {
        Serial.println("ERROR: Invalid angle (0-360)");
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
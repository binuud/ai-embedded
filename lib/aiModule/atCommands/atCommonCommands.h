#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>
#include <iotActuators.h>



#include <serialHandler.h>

void atCmdClearStorage(const String& args);


void atCmdLedConfig(const String& args);
void atCmdConfig(const String& args);

void atReadLedConfig();
void atReadConfig();



// 2. A separate module initialization function that populates your command class instance
void registerCommonATCommands(SerialHandler &cmdManager) {
    
    // Construct individual instances
    AtCommand ledCmd =       {"LED", "Config Led Pin, AT+LED=[LED NUMBER], [LED_PIN]" , atCmdLedConfig, atReadLedConfig};
    AtCommand storeageCmd =  {"CLEAR_STORAGE", "Clear all stored values", atCmdClearStorage, nullptr};
    
    // Push them into the manager instance
    cmdManager.add(ledCmd);
    cmdManager.add(storeageCmd);
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



void atCmdClearStorage(const String& params) {
    devicePrefs.clear();
    Serial.println("Cleared storage");
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


void atReadConfig() {
    Serial.print("Stepper angle : \n");    
}

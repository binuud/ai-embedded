#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>
#include <iotActuators.h>



#include <serialHandler.h>

void atReadBleName();



// 2. A separate module initialization function that populates your command class instance
void registerBleATCommands(SerialHandler &cmdManager) {
    
    // Construct individual instances
     AtCommand bleNameCmd =  {"BLE_NAME", "Config BLE Name, eg: AT+BLE_NAME=[BLE-NAME]", nullptr, atReadBleName};
        
    
    // Push them into the manager instance
    cmdManager.add(bleNameCmd);

}

// void atCmdBleName(const String& params) {
//     if (params.length() > 0) {
//         devicePrefs.config.ble_name = params;
//         devicePrefs.saveBle();
//         Serial.printf("atCmdBleName new name %s , OK\n", params);
//     } else {
//         Serial.println("ERROR: BLE name required");
//     }
// }

void atReadBleName() {
    Serial.printf("Ble name : %s \n", devicePrefs.config.ble_name);    
}


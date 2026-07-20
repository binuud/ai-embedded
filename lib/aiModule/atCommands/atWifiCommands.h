#ifndef NETWORK_COMMANDS_H
#define NETWORK_COMMANDS_H

#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>
#include <iotActuators.h>
#include <wifiInit.h>


#include <serialHandler.h>

// 1. Define dummy callback functions for your hardware actions
void atCmdWifiSSID(const String& args);
void atCmdWifiPass(const String& args);
void atCmdWifiConnect(const String& args);

void atReadWifiSSID();
void atReadWifiPass();
void atCmdWifiConnectStatus();

// 2. A separate module initialization function that populates your command class instance
void registerWifiATCommands(SerialHandler &cmdManager) {
    
    // Construct individual instances
    AtCommand ssidCmd = {"WIFI_SSID", "Config Wifi SSID, eg: AT+WIFI_SSID=[WIFI-SSID]", atCmdWifiSSID, atReadWifiSSID};
    AtCommand passCmd = {"WIFI_PASS", "Config Wifi SSID, eg: AT+WIFI_PASS=[WIFI-PASS]", atCmdWifiPass, atReadWifiPass};
    AtCommand reconnectCmd = {"WIFI_CONNECT", "Connect, eg: AT+WIFI=CONNECT", atCmdWifiConnect, atCmdWifiConnectStatus};     
    
    // Push them into the manager instance
    cmdManager.add(ssidCmd);
    cmdManager.add(passCmd);
    cmdManager.add(reconnectCmd);
}

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

void atCmdWifiConnect(const String& params) {
    
}

void atCmdWifiConnectStatus() {
    printWifiStatus();
}


void atReadWifiSSID() {
    Serial.printf("Wifi ssid : %s \n", devicePrefs.config.wifi_ssid);    
}


void atReadWifiPass() {
    Serial.printf("Wifi Password : %s \n", devicePrefs.config.wifi_password);    
}


#endif
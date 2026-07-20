#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

// for parsing commands from serial and invoking same
// using the AT+ command structure
// AT+[CMD]? for reading values
// AT+[CMD]=param1, param2 for setting values
// Binu Udayakumar (binu@dronasys.com)

// this is prefix match, commands coming first, will take precendence in prefix match

#include <helpers.h>
#include <iotCmd.h>


class SerialHandler {
private:
    static const int MAX_COMMANDS = 10; // Adjust memory ceiling here
    AtCommand atCommands[MAX_COMMANDS];
    int _count;

public:


    // SerialHandler(AtCommand atCommandsArray[], int count) {
    //     atCommands = atCommandsArray;
    //     atCommandCount = count;
    // }

    SerialHandler(int count) {
        
    }



    // Method to push an instance of a command into the collection
    bool add(AtCommand cmd) {
        if (_count < MAX_COMMANDS) {
            atCommands[_count] = cmd;
            _count++;
            return true;
        }
        return false; // Out of memory space
    }

    void help() {

        Serial.printf("Available Registered commands  %d :", _count);
        Serial.println("  AT? -- help message");
        Serial.println("  All set commands will have the AT command followed by = symbol and values");
        Serial.println("  All read commands will have just the AT command without any further symbols or values");
        for (int i = 0; i < _count; i++) {
            Serial.println(String("  AT+") + atCommands[i].name + "=<params>");
            Serial.println(String("    Desc: ") + atCommands[i].desc);
        }
        Serial.println("OK");

    }

    void loop() {

        if (Serial.available() > 0) {
            
            String input = Serial.readStringUntil('\n');
            input.trim();

            String prefix = input.substring(0, 4);  // Get the first four characters
            prefix.toUpperCase();   

            
            if (!prefix.startsWith("AT")) {
                Serial.println("ERROR: Commands must start with 'AT'");
                return;
            }

            // Handle help command if input is exactly "AT?" or "AT+?"
            if (prefix == "AT?" || prefix == "AT+?") {
                help();
                return;
            }

            // Remove the "AT" prefix
            String cmdLine = input.substring(3);
            cmdLine.trim();

            if (cmdLine.length() == 0) {
                Serial.println("OK");
                return;  // Just "AT" command, respond OK
            }
            cmdLine.toUpperCase();  // Convert cmdName to lowercase
            // Find command in atCommands array
            bool handled = false;
            for (int i = 0; i < MAX_COMMANDS; i++) {
                String cmdName = atCommands[i].name;

                if (cmdLine.startsWith(cmdName)) {
                    String params = input.substring(3+cmdName.length()); // to prevent auto uppercase, done above to influence the parameters
                    params.trim();

                    // If parameters start with '=', remove it and pass rest as params
                    if (params.startsWith("=")) {
                        params = params.substring(1);
                        params.trim();
                        if (atCommands[i].execCallback != nullptr) {
                            atCommands[i].execCallback(params);
                        }
                        
                        handled = true;
                    } else {
                        // No parameters part after command
                        // treat it as a read command
                        if (atCommands[i].readCallback != nullptr) {
                            atCommands[i].readCallback();
                        }
                        
                        handled = true;
                    }

                    break;
                }
            }

            if (!handled) {
                Serial.println("Unknown command. Available commands: ANGLE, WIFI_SSID, WIFI_PASS, CONFIG.");
            }
        }    
    }

};


#endif // SERIAL_HANDLER_H
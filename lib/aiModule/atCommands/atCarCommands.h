#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>
#include <iotActuators.h>
#include <serialHandler.h>

#if CAR_FIXED_STEERING
#include <iotCarFixedSteering.h>
#endif

#if CAR_FLUID_STEERING
#include <iotCarFluidSteering.h>
#endif

void atCmdCarConfig(const String& args);
void atCmdCarMove(const String& args);

void atReadCarConfig();
void atReadCarMove();


// 2. A separate module initialization function that populates your command class instance
void registerCarATCommands(SerialHandler &cmdManager) {
    
    // Construct individual instances
        
    AtCommand carConfigCmd =  {"CAR", "Config Car control Pins, eg: AT+CAR=[MotorLeft+], [MotorLeft-], [MotorRight+], [MotorRight-]" , atCmdCarConfig, atReadCarConfig};
    AtCommand carMoveCmd =  {"MOVE", "Move car, AT+MOVE=[+-]SPEED,[+-], eg: AT+MOVE=50,50 will move card forward and turn right, AT+MOVE=-50,0 will move car back", atCmdCarMove, atReadCarMove};
    // Push them into the manager instance
    cmdManager.add(carConfigCmd);
    cmdManager.add(carMoveCmd);
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



void atCmdCarMove(const String& params) {
    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        int numParts = splitString(params, ',', parts, 2);
        if (numParts == 2) {
 
            int speed = parts[0].toInt();
            if (speed > 100) speed = 100;
            if (speed < -100) speed = -100;
            int turn = parts[1].toInt();
            if (turn > 100) turn = 100;
            if (turn < -100) turn = -100;
            if (speed == 0 && turn == 0 ) {
                carStop();
            } else {
                move(speed, turn); 
            }
            Serial.printf("Car command speed %d, turn  %d \n", speed, turn);
        } else {
            Serial.println("ERROR: Invalid format");
            Serial.println("ERROR: invalid params, AT+MOVE=SPEED,TURN eg: AT+MOVE=50,0");
        }
    } else {
        Serial.println("ERROR: invalid params, AT+MOVE=SPEED,TURN eg: AT+MOVE=50,0");
    }    
}


// void atCmdCarMove(const String& params) {
//     if (params.length() > 0) {
//         String parts[2]; // adjust size as needed
//         int numParts = splitString(params, ',', parts, 2);
//         if (numParts == 2) {
//             String action = parts[0];
//             action.toUpperCase();   
//             int speed = parts[1].toInt();
//             if (speed > 100) speed = 100;
//             if (speed < 0) speed = 0;
//             setSpeed(speed);
//             Serial.printf("Car command %s, %d \n", action, speed);
//             if (action == "W") move(speed, 0); 
//             else if (action == "S") move(-speed, 0);
//             else if (action == "A") move(0, -100);
//             else if (action == "D") move(0, 100);
//             else if (action == "Z") carStop();
//         } else {
//             Serial.println("ERROR: Invalid format");
//         }
//     } else {
//         Serial.println("ERROR: invalid params, format Direction[WASDZ], speed");
//     }    
// }

void atReadCarMove() {
    Serial.print("Not Implemented : \n");    
}

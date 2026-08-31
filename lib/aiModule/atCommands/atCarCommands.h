#include <iotCmd.h>

#include <serialHandler.h>
#include <boardState.h>
#include <iotActuators.h>
#include <serialHandler.h>

#if CAR_DIFFERENTIAL_DRIVE
#include <iotCarDifferentialDrive.h>
#endif

#if CAR_DIFFERENTIAL_DRIVE
#include <iotCarFluidSteering.h>
#endif

void atCmdCarConfig(const String& args);
void atCmdCarMove(const String& args);
void atCmdCarVector(const String& args);
void atReadCarConfig();
void atReadCarMove();
void atReadCarStatus();

// 2. A separate module initialization function that populates your command class instance
void registerCarATCommands(SerialHandler &cmdManager) {
    
    // Construct individual instances
        
    AtCommand carConfigCmd =  {"CAR", "Config Car control Pins, eg: AT+CAR=[MotorLeft+], [MotorLeft-], [MotorRight+], [MotorRight-]" , atCmdCarConfig, atReadCarConfig};
    AtCommand carMoveCmd =  {"MOVE", "Move car, AT+MOVE=[+-]SPEED,[+-], eg: AT+MOVE=50,50 will move card forward and turn right, AT+MOVE=-50,0 will move car back", atCmdCarMove, atReadCarMove};
    AtCommand carVecMoveCmd =  {"VECMOVE", "Move car, AT+VECMOVE=[+-]VECTOR,[+-]ANGLE, eg: AT+MOVE=50,50 will move card forward and turn right, AT+VECMOVE=-50,0 will move car back", atCmdCarVector, atReadCarMove};
    AtCommand carStatusCmd =  {"CARSTATUS", "Car Status, AT+CARSTATUS=", nullptr, atReadCarStatus};
    
    // Push them into the manager instance
    cmdManager.add(carConfigCmd);
    cmdManager.add(carMoveCmd);
    cmdManager.add(carVecMoveCmd);
    cmdManager.add(carStatusCmd);
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
            return;
        } 
    }
        
    Serial.println("ERROR: invalid params, AT+MOVE=SPEED,TURN eg: AT+MOVE=50,0");
        
}

// atCmdCarVector
// vector based movement, received 2 entries, first one is velocity and second is angular vector
void atCmdCarVector(const String& params) {
    if (params.length() > 0) {
        String parts[2]; // adjust size as needed
        int numParts = splitString(params, ',', parts, 2);
        if (numParts == 2) {
 
            int velocity = parts[0].toInt();
            // if (velocity > 100) velocity = 100;
            // if (velocity < -100) velocity = -100;
            int angular = parts[1].toInt();
            // if (angular > 100) angular = 100;
            // if (angular < -100) angular = -100;

            moveWithVector(velocity, angular);
            Serial.printf("Car command vecolity %d, angle  %d \n", velocity, angular);
            return;
        } 
    } 
    Serial.println("ERROR: invalid params, AT+VECMOVE=VELOCITY,ANGLE eg: AT+VECMOVE=50,0");
       
}

void atReadCarStatus() {
    Serial.printf("Compass reading %3.2f", getCompassReading());
}

void atReadCarMove() {
    Serial.print("Not Implemented : \n");    
}

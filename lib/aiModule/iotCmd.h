#ifndef IOT_CMD_H
#define IOT_CMD_H

// Binu Udayakumar
// The type structures have to be similar on client side to
// we are using this format, to allow BLE to transmit and control the Bots and devices
// json and string are cumbersome for this functionality
// I am planning for LORA and satellite transmission as well.
// overrun 8 bit data soon, using 16 bit for all data

#include <HardwareSerial.h>

enum DeviceCategory   {
  DeviceCategory_ignore,
  DeviceCategory_unknown,
  DeviceCategory_move,
  DeviceCategory_car_2_wheel_module,
  DeviceCategory_servo,
  DeviceCategory_stepper,
  DeviceCategory_led,
  DeviceCategory_led_strip,
};

enum SubCmdEnum   {
  SubCmdEnum_ignore,
  SubCmdEnum_vec_move, // vector move
  SubCmdEnum_move,  // get value 1 for forward (positive), backward speed (negative)
                    // get value 2 for steer right (positive), steer left (negative)
  SubCmdEnum_move_forward,
  SubCmdEnum_move_backward,
  SubCmdEnum_move_turn_left,
  SubCmdEnum_move_turn_right,
  SubCmdEnum_move_strafe_left,
  SubCmdEnum_move_strafe_right,
  SubCmdEnum_move_center_steer,
  SubCmdEnum_move_stop,

  SubCmdEnum_servo_angle,
  SubCmdEnum_servo_return,
  SubCmdEnum_servo_max,

  SubCmdEnum_stepper_angle,

  SubCmdEnum_led_on,
  SubCmdEnum_led_off,

  SubCmdEnum_led_strip_on,
  SubCmdEnum_led_strip_off,
  SubCmdEnum_led_strip_color,
};


// Structure using enums
typedef struct {
  DeviceCategory cmd;
  SubCmdEnum subcmd;
  
  // if there are multiple IO types, like if we have 3 LED, 
  // this is used to identify the led, on which the action has to be associated with
  int32_t identifier; 
  
  int32_t value1;
  int32_t value2;
  int32_t value3;
} IotCommand;


// Struct for command and its callback

typedef void (*CmdCallback)(const String& args);
typedef void (*CmdReadCallback)();

struct AtCommand {
    const char* name;   // the actual AT command without the suffix and prefix
    const char* desc;   // description of the command, that will be displayed to the user
    CmdCallback execCallback;       // invoked when the command is called with suffix "=", for configuring the device
    CmdReadCallback readCallback;   // invoked when the command is called in read mode, with suffix "?", there are no parameters in read mode
};

static inline uint32_t decodeUInt32LE(const uint8_t* data, size_t startIndex) {
    return  static_cast<uint32_t>(data[startIndex + 0]) |
           (static_cast<uint32_t>(data[startIndex + 1]) << 8) |
           (static_cast<uint32_t>(data[startIndex + 2]) << 16) |
           (static_cast<uint32_t>(data[startIndex + 3]) << 24);
}

static inline int32_t decodeInt32LE(const uint8_t* data, size_t startIndex) {
    return static_cast<int32_t>(decodeUInt32LE(data, startIndex));
}

void printPayload(const uint8_t* rawDataBuffer) {
  Serial.printf(" Payload size %d", sizeof(rawDataBuffer));
  for (int i=0; i<sizeof(rawDataBuffer); i++) {
    Serial.printf("%02x ", rawDataBuffer[i]);
    
  }
  Serial.println();
}

// Decoder function to parse raw uint8_t buffer into a Command struct
void decodeCommand(const uint8_t* data, IotCommand* cmd) {
  
  if (cmd == nullptr) return; 
  cmd->cmd = DeviceCategory_ignore;

  if (data == nullptr) {
    Serial.printf("Invalid stream for decodeCommand \n");
    return;
  }

  // printPayload(data);

  cmd->cmd = static_cast<DeviceCategory>(decodeUInt32LE(data, 0));
  cmd->subcmd = static_cast<SubCmdEnum>(decodeUInt32LE(data, 4));
  cmd->identifier = decodeInt32LE(data, 8);
  cmd->value1 = decodeInt32LE(data, 12);
  cmd->value2 = decodeInt32LE(data, 16);
  cmd->value3 = decodeInt32LE(data, 20);

  // Here assuming little-endian 16-bit ints, adjust if different

  Serial.printf("Last decodeCommand %d:%d  ( %d, %d, %d ) \n", cmd->cmd, cmd->subcmd, cmd->value1, cmd->value2, cmd->value3);

}


void debugIotCommand(IotCommand* cmd) {
  if (cmd == nullptr) return; 

  Serial.printf("Cmd: %5d, Sub: %3d, Identifier: %3d Val1: %3d, Val2: %3d, Val3: %3d\n",
                cmd->cmd, cmd->subcmd, cmd->identifier, cmd->value1, cmd->value2, cmd->value3);

}

#endif // IOT_CMD_H
#ifndef IOT_CMD_H
#define IOT_CMD_H

// Binu Udayakumar
// The type structures have to be similar on client side to
// we are using this format, to allow BLE to transmit and control the Bots and devices
// json and string are cumbersome for this functionality
// I am planning for LORA and satellite transmission as well.
// Client Side will hold the logic for 8 bit or 16 bit or 32 bit arch
// this can be done with a simple case login on the encoder on the client side
#include <HardwareSerial.h>

enum CmdEnum {
  CmdEnum_ignore,
  CmdEnum_Move,
  CmdEnum_Servo,
  CmdEnum_Led,
  CmdEnum_LedStrip,
};

enum SubCmdEnum {
  SubCmdEnum_ignore, 
  SubCmdEnum_move_forward,
  SubCmdEnum_move_backward,
  SubCmdEnum_move_turn_left,
  SubCmdEnum_move_turn_right,
  SubCmdEnum_move_strafe_left,
  SubCmdEnum_move_strafe_right,
  SubCmdEnum_move_stop,

  SubCmdEnum_servo_angle,
  SubCmdEnum_servo_return,
  SubCmdEnum_servo_max,

  SubCmdEnum_led_on,
  SubCmdEnum_led_off,

  SubCmdEnum_led_strip_on,
  SubCmdEnum_led_strip_off,
  SubCmdEnum_led_strip_color,
};


// Structure using enums
typedef struct {
  CmdEnum cmd;
  SubCmdEnum subcmd;
  
  // if there are multiple IO types, like if we have 3 LED, 
  // this is used to identify the led, on which the action has to be associated with
  int identifier; 
  
  int value1;
  int value2;
  int value3;
} IotCommand;

// Decoder function to parse raw uint8_t buffer into a Command struct
void decodeCommand(const uint8_t* data, IotCommand* cmd) {
  
  if (cmd == nullptr) return; 

  cmd->cmd = CmdEnum_ignore;

  if (data == nullptr) {
    Serial.printf("Invalid stream for decodeCommand \n");
    return;
  }
  
  // Decode enums and integer values from the incoming byte array
  cmd->cmd = static_cast<CmdEnum>( data[0] );
  cmd->subcmd = static_cast<SubCmdEnum>( data[1] );

  // Assuming 2 bytes per int, decode identifier and values (big-endian or little-endian as needed)
  // Here assuming little-endian 16-bit ints, adjust if different
  cmd->identifier = data[2];
  cmd->value1 = data[3];
  cmd->value2 = data[4];
  Serial.printf("Last but one line of decodeCommand \n");
  cmd->value3 = data[5];
  Serial.printf("Last line of decodeCommand %d \n", cmd->value3);
}

void debugIotCommand(IotCommand* cmd) {
  if (cmd == nullptr) return; 

  Serial.printf("Cmd: %5d, Sub: %3d, Identifier: %3d Val1: %3d, Val2: %3d, Val3: %3d\n",
                cmd->cmd, cmd->subcmd, cmd->identifier, cmd->value1, cmd->value2, cmd->value3);
}

#endif // IOT_CMD_H
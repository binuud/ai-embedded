// Binu Udayakumar
// The type structures have to be similar on client side to
// we are using this format, to allow BLE to transmit and control the Bots and devices
// json and string are cumbersome for this functionality
// I am planning for LORA and satellite transmission as well.
// Client Side will hold the logic for 8 bit or 16 bit or 32 bit arch
// this can be done with a simple case login on the encoder on the client side

enum CmdEnum {
  Ignore,
  Move,
  Servo,
  Led,
  LedStrip,
};

enum SubCmdEnum {
  Ignore, 
  Move_forward,
  Move_backward,
  Move_turn_left,
  Move_turn_right,
  Move_strafe_left,
  Move_strafe_right,
  Move_stop,

  Servo_angle,
  Servo_return,
  Servo_max,

  Led_on,
  Led_off,

  LedStrip_on,
  LedStrip_off,
  LedStrip_color,
};


// Structure using enums
struct IotCommand {
  CmdEnum cmd;
  SubCmdEnum subcmd;
  
  // if there are multiple IO types, like if we have 3 LED, 
  // this is used to identify the led, on which the action has to be associated with
  int identifier; 
  
  int value1;
  int value2;
  int value3;
};

// Decoder function to parse raw uint8_t buffer into a Command struct
IotCommand decodeCommand(const uint8_t* data) {
  
  IotCommand cmd;
  
  // Decode enums and integer values from the incoming byte array
  cmd.cmd = static_cast<CmdEnum>(data[0]);
  cmd.subcmd = static_cast<SubCmdEnum>(data[1]);

  // Assuming 2 bytes per int, decode identifier and values (big-endian or little-endian as needed)
  // Here assuming little-endian 16-bit ints, adjust if different
  cmd.identifier = data[2] | (data[3] << 8);
  cmd.value1 = data[4] | (data[5] << 8);
  cmd.value2 = data[6] | (data[7] << 8);
  cmd.value3 = data[8] | (data[9] << 8);

  return cmd;
}

void debugIotCommand(const Command& cmd) {
  Serial.printf("Cmd: %5s, Sub: %3dd, Identifier: %3d Val1: %3d, Val2: %3d, Val3: %3d \n", cmd.cmd, cmd.subcmd, cmd.identifier, cmd.valu1, cmd.value2, cmd.value3);
}
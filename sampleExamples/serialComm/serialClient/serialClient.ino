#include <SoftwareSerial.h>

// UART communication

// Define custom RX and TX pins (RX=Pin 10, TX=Pin 11)
SoftwareSerial mySerial(10, 11); 

void setup() {
  Serial.begin(9600);   // Hardware UART for computer debugging
  mySerial.begin(9600); // Software UART for external device
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read()); // Pass data from module to computer
  }
}
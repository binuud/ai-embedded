#include <SoftwareSerial.h>

// UART communication

// Define custom RX and TX pins (RX=Pin 10, TX=Pin 11)
SoftwareSerial mySerial(10, 11); 


int count = 0;

void setup() {
  
  // DEBUGGING - SERIAL MONITOR ON COMPUTER
  Serial.begin(9600);   // Hardware UART for computer debugging 

  mySerial.begin(9600); // Software UART for external device

}

void loop() {
  count ++;
  if (mySerial.available()) {
    Serial.write(mySerial.read()); // Pass data from module to computer
    byte lowByte = lowByte(count);   // Extracts the lower 8 bits
    byte highByte = highByte(count); // Extracts the upper 8 bits
  }
  delay(500);
}
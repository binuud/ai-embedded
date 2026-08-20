#include <HardwareSerial.h>
#include <Arduino.h>

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600); 
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB boards like Leonardo/Micro)
  }
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read the incoming string until it encounters a newline character '\n'
    String receivedDate = Serial.readStringUntil('\n');
    
    // Trim any accidental whitespace or carriage returns
    receivedDate.trim(); 
    
    // Print a confirmation back to the serial monitor/Pi
    Serial.print("Arduino received date: ");
    Serial.println(receivedDate);
  }
}
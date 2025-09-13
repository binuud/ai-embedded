#include <SoftwareSerial.h>

SoftwareSerial us100Serial(21, 22); // RX, TX connected to US-100 TX (echo) and RX (trigger) pins

void setup() {
  Serial.begin(115200);
  us100Serial.begin(9600);
  Serial.println("US-100 sensor reading distance and temperature");
}

void loop() {
  // Read distance
  us100Serial.write(0x55); // Command to request distance
  delay(100);
  if (us100Serial.available() >= 2) {
    int highByte = us100Serial.read();
    int lowByte = us100Serial.read();
    int distanceMM = highByte * 256 + lowByte;
    Serial.print("Distance: ");
    Serial.print(distanceMM / 10.0);
    Serial.println(" cm");
  } else {
    Serial.println("Distance read failed");
  }

  delay(200);

  // Read temperature
  us100Serial.write(0x50); // Command to request temperature
  delay(100);
  if (us100Serial.available() >= 1) {
    int tempRaw = us100Serial.read();
    int temperatureC = tempRaw - 45;
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
  } else {
    Serial.println("Temperature read failed");
  }

  delay(1000);
}

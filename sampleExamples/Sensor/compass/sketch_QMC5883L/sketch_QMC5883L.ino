#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // ESP32 default SDA=21, SCL=22 for I2C

  compass.init();
  Serial.println("QMC5883L Compass Initialized");
}

void loop() {
  compass.read();

  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();

  int a = compass.getAzimuth();

  Serial.print("Azimuth : ");
  Serial.print(a);

  Serial.print("Mag X: ");
  Serial.print(x);
  Serial.print(" \tMag Y: ");
  Serial.print(y);
  Serial.print(" \tMag Z: ");
  Serial.println(z);

  delay(500);
}

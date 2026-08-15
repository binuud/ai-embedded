#include <Wire.h>
#include "DFRobot_BMM150.h"

// Define I2C pins for ESP32 (change if needed)
const int SDA_PIN = 22; // 8; // 44; //  13;
const int SCL_PIN = 21; //9; //43; // 14;

// Create BMM150 object, I2C address typically 0x13 (I2C_ADDRESS_4 by default)
DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C bus with ESP32 pins

  while(bmm150.begin()) { // Initialize sensor, retry if failed
    Serial.println("bmm150 init failed, Please try again!");
    delay(1000);
  }
  Serial.println("bmm150 init success!");

  bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);      // Normal mode
  bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY); // High accuracy preset
  bmm150.setRate(BMM150_DATA_RATE_10HZ);                 // Set data rate to 10Hz
  bmm150.setMeasurementXYZ();  
  
  Serial.println("Compass data");                          // Enable X, Y, Z axes measurements
}

void loop() {
  sBmm150MagData_t magData = bmm150.getGeomagneticData(); // Read magnetic data
  
  float compassDegree = bmm150.getCompassDegree(); // Get compass heading in degrees
  Serial.printf("x,y,x (%d,%d,%d) Compass: %f", magData.x, magData.y, magData.z, compassDegree);
  Serial.print("   \n");
  // Serial.print("mag x = "); Serial.print(magData.x); Serial.println(" uT");
  // Serial.print("mag y = "); Serial.print(magData.y); Serial.println(" uT");
  // Serial.print("mag z = "); Serial.print(magData.z); Serial.println(" uT");


  Serial.print("Compass heading (degrees): ");
  Serial.println(compassDegree);

  Serial.println("--------------------------------");
  delay(1500);
}

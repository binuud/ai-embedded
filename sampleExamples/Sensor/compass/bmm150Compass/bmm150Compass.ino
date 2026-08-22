#include <Wire.h>
#include "DFRobot_BMM150.h"

#define ARDUINO_BOARD 1
#define ESP_BOARD 0

// Define I2C pins for ESP32 (change if needed)
#if ESP_BOARD
const int SDA_PIN = 21; // 8; // 44; //  13;
const int SCL_PIN = 22; //9; //43; // 14;
#endif

// Create BMM150 object, I2C address typically 0x13 (I2C_ADDRESS_4 by default)
DFRobot_BMM150_I2C bmm150(&Wire, I2C_ADDRESS_4);

void setup() {





#if ESP_BOARD
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C bus with ESP32 pins
#endif

#if ARDUINO_BOARD
  Serial.begin(9600);
  Wire.begin(); // Initialize I2C bus with A4, A5  pins of arduino uno
#endif

  while(bmm150.begin()) { // Initialize sensor, retry if failed
    Serial.println("bmm150 init failed, Please try again!");
    delay(500);
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

#if ARDUINO_BOARD  

  Serial.print("x,y,x, Compass ");
  Serial.print("X (uT): "); Serial.print(magData.x);
  Serial.print(" | Y (uT): "); Serial.print(magData.y);
  Serial.print(" | Z (uT): "); Serial.print(magData.z);
  Serial.print(" | Compass : "); Serial.println(compassDegree);

#else  
  Serial.printf("x,y,x (%d,%d,%d) Compass: %f", magData.x, magData.y, magData.z, compassDegree);
  Serial.print("   \n");

#endif  

  delay(1000);
}

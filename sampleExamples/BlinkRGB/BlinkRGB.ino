/*
  BlinkRGB

  Demonstrates usage of onboard RGB LED on some ESP dev boards.

  Calling digitalWrite(RGB_BUILTIN, HIGH) will use hidden RGB driver.

  RGBLedWrite demonstrates control of each channel:
  void rgbLedWrite(uint8_t pin, uint8_t red_val, uint8_t green_val, uint8_t blue_val)

  WARNING: After using digitalWrite to drive RGB LED it will be impossible to drive the same pin
    with normal HIGH/LOW level
*/
#define RGB_BUILTIN 2 // GPIO2
#define RGB_BRIGHTNESS 64 // Change white brightness (max 255)

// the setup function runs once when you press reset or power the board

void setup() {
  Serial.begin(115200); 
  pinMode(RGB_BUILTIN, OUTPUT);  // Initialize the LED pin as output
}

// the loop function runs over and over again forever
void loop() {
#ifdef RGB_BUILTIN
  digitalWrite(RGB_BUILTIN, LOW);
  Serial.println("Looping White... ");
  rgbLedWrite(RGB_BUILTIN, HIGH, HIGH, HIGH);  // Off / black
  delay(2000);
  Serial.println("Looping color... ");
  rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, 0);  // Turn the RGB LED white
  delay(1000);
  Serial.println("Looping off... ");
  digitalWrite(RGB_BUILTIN, LOW);  // Turn the RGB LED off
  delay(1000);
  Serial.println("Looping Red... ");
  rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);  // Red
  delay(1000);
  Serial.println("Looping Green... ");
  rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);  // Green
  delay(1000);
  Serial.println("Looping Blue... ");
  rgbLedWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS);  // Blue
  delay(1000);
  Serial.println("Looping Off... ");
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // Off / black
  delay(1000);
  Serial.println("Looping End... ");

#endif
}


const int DIGITAL_PIN1 = 25;  // GPIO 34 (ADC1_CH6)
const int DIGITAL_PIN2 = 32; // GPIO 15

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  
  // Configure pin modes
  pinMode(DIGITAL_PIN1, INPUT_PULLUP);
  pinMode(DIGITAL_PIN2, INPUT_PULLUP); // Keeps digital pin stable at 0 when unpressed
}

void loop() {
  // Read values from the pins
  int digitalVal1 = digitalRead(DIGITAL_PIN1); // Returns 0 to 4095 (12-bit ADC)
  int digitalVal2 = digitalRead(DIGITAL_PIN2); // Returns 0 or 1
  
  // Format data for the Arduino Serial Plotter
  // Format rule: "Label1:Value1,Label2:Value2" followed by a newline
  Serial.print("DigitalVal1:");
  Serial.println(digitalVal1 * 1000); 
  Serial.print(",");
  Serial.print("DigitalVal2:");
  Serial.println(digitalVal2 * 1000); // Scaled up by 1000 so it's visible next to the analog data
  
  // Sample rate control (approx. 50 updates per second)
  delay(20); 
}
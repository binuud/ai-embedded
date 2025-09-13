// Example pin assignments (replace with your actual pins)
#define CLK_PIN 17
#define DIR_PIN 5
#define EN_PIN 16
int count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  Serial.println("Stepper Initialized");
}

void loop() {
  count ++;
  // Move motor forward
  digitalWrite(DIR_PIN, HIGH); // Set direction
  digitalWrite(EN_PIN, LOW);   // Enable driver
  for (int i = 0; i < 200; i++) { // Example: move 200 steps
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(EN_PIN, HIGH); // Disable driver
  delay(2000); // Wait 2 seconds

  // Move motor backward
  digitalWrite(DIR_PIN, LOW);  // Set direction
  digitalWrite(EN_PIN, LOW);   // Enable driver
  for (int i = 0; i < 200; i++) { // Example: move 200 steps
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(EN_PIN, HIGH); // Disable 
  Serial.printf("looping %d\n", count);

  delay(2000); // Wait 2 seconds
}

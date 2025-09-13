#define stepPin 5
#define dirPin 17
#define enablePin 18

const int stepsPerRevolution = 1600; // Change this according to your microstep setting
int count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW); 
  Serial.println("Stepper Initialized");
}

void loop() {
  count ++;
  Serial.printf("looping  clock wise  %d\n", count);
  digitalWrite(dirPin, HIGH); 
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500); // Adjust delay to control speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000); 
  Serial.printf("looping Counter clock wise %d\n", count);
  digitalWrite(dirPin, LOW); 
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500); // Adjust delay to control speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(10000); 
}
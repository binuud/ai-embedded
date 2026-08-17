
const int ENCODER_A = 8;
const int ENCODER_B = 9;

int pulsesChanged = 0;

// Use volatile for variables modified inside interrupts
long encoderCount = 0;
static long lastCount = 0;
// Interrupt Service Routine (ISR) for Phase A


// Interrupt Service Routine (ISR) for Phase B
void ISR_B() {

  int a = digitalRead(ENCODER_A);
  if (a == HIGH) {
    encoderCount++;
  } else  {
    encoderCount--;
  }

  pulsesChanged = 1;
  Serial.print("ChannelB:");
  Serial.println(a);

}

void setup() {

  Serial.begin(9600);

  // Configure encoder pins with internal pull-up resistors
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);

  // Attach interrupts with IRAM_ATTR attribute for ESP32 speed
  // attachInterrupt(digitalPinToInterrupt(ENCODER_A), ISR_A, RISING);

  attachInterrupt(digitalPinToInterrupt(ENCODER_B), ISR_B, CHANGE);

  Serial.println("ESP32 Encoder Initialized.");

}

void loop() {

  // Serial.printf("ChannelB:%d\n", pulsesChanged*1000);
  // Only print when the position changes to avoid serial flooding
  // if (encoderCount != lastCount) {
  //   lastCount = encoderCount;
  //   Serial.print("Position: ");
  //   Serial.println(encoderCount);
  // }

  delay(1000); // Small stability delay

}

// const int DIGITAL_PIN1 = 25;  // GPIO 34 (ADC1_CH6)
// const int DIGITAL_PIN2 = 32; // GPIO 15

const int ENCODER_A = 21;
const int ENCODER_B = 20;

volatile int pulsesChanged = 0;

// Use volatile for variables modified inside interrupts
volatile long encoderCount = 0;
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

}

void setup() {
  Serial.begin(115200);

  // Configure encoder pins with internal pull-up resistors
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // Attach interrupts with IRAM_ATTR attribute for ESP32 speed
  // attachInterrupt(digitalPinToInterrupt(ENCODER_A), ISR_A, RISING);

  attachInterrupt(digitalPinToInterrupt(ENCODER_B), ISR_B, CHANGE);

  Serial.println("ESP32 Encoder Initialized.");
}

void loop() {

  // Serial.printf("ChannelB:%d\n", pulsesChanged*1000);
  // Only print when the position changes to avoid serial flooding
  if (pulsesChanged) {

    pulsesChanged = 0;
      // 
      if (encoderCount != lastCount) {
        lastCount = encoderCount;
        Serial.print("Position: ");
        Serial.println(lastCount);
      }
  }

   Serial.println(lastCount);
  delay(1000); // Small stability delay
}
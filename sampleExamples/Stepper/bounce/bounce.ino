#include <AccelStepper.h>


// Define a stepper motor 1 for arduino 
// direction Digital 9 (CW), pulses Digital 8 (CLK)
AccelStepper stepper(1, 17, 5); // clk cw
int count = 0;
void setup()
{  
  Serial.begin(115200);
  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(1000);//1100
  stepper.setAcceleration(1100);
  stepper.moveTo(2000);
  Serial.println("Stepper Motor Control with ESP32");
}

void loop()
{
    count ++;
    Serial.printf("Rotating %d...\n", count);
    // If at the end of travel go to the other end
    if (stepper.distanceToGo() == 0){
      stepper.moveTo( -stepper.currentPosition() );
    }
    
    stepper.run();
    delay(1000);
}
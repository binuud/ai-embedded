## Samples and experiments

* Using different sensors and brands was taking too much time
* Finalized list of sensors
* Finalized list of motor drivers that work in 6 to 12V

This folder contains test sketch files, to be launched and deployed from Arduino IDE.

## Findings

* H Bridge DC motor driver is better than L298N
* Serial is better than using lot of PWM pins
* ESP32 is much more cost effective than other embedded control units
* BLE has a size limit of 250bytes for transmission
* Raspberry PI is not the device for driving motors, we need a arduino in between, else we run out of PWM pins.
* Not all GPIO listed by vendor are GPIO, test test test.
* Micropython speed and performance is an issue, as code complicates.
* Some vendors bake in compass, accelerometer, and servo drive on same board, its much simpler to build on this.
* Using breadboard on moving devices results in loose contact.
* Do not drive motors directly from dev board, unless it has a driver and external power.



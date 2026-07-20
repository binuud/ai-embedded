# ai-embedded
BrainUI module for Esp32 and Arduino. 

<!-- TOC created by git hub actions>
<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Directory Structure](#directory-structure)
- [Geting Started](#geting-started)
- [AT Commands Help](#at-commands-help)
- [API Endpoints for streaming](#api-endpoints-for-streaming)
- [Communication CMD Structure](#communication-cmd-structure)
- [Types of Steering](#types-of-steering)
- [Motor Driver Support](#motor-driver-support)
- [SOC Support](#soc-support)
- [Code Editors](#code-editors)
- [Build Environments](#build-environments)
- [Points to remember](#points-to-remember)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Directory Structure
* /deviceDetails - board pinout details
* /lib - library for handling Wifi, AT commands, BLE, drivers for motors
* /sampleExamples - quick run code for Arduino.ide to test basic functionality like drivers, signalling, I2c, display boards etc
* /partitions - csv files for EEPROM partitions
* /src - multiple projects share this workspace, each being a child folder of /src
    * /esp32CarWifiWithCamera - Bluetooth car control (BLE), with steering dc motor and one drive motor, with variable speed
    * /esp32CarWifiWithCamera - Wifi Car Control, with streaming camera, differential steering, with variable speed
    * /basicBleWifi - To check basi BLE and Wifi Configurations

## Geting Started

* Clone this project
* Use Visual Code Editor
* Install PlatformIO IDE extension 
* Select environment of your choice
* Build the project (Tick mark on footer of editor)
* Connect ESP32 Dev Board, choose the device on the footer of the editor
* Deploy the code (Right arrow mark on footer of editor)
* [Project Info](https://binuud.com/project/smallrobots/)
* [Access UI](https://binuud.com/staging/robot)
    * Please note, open UI url in Google Chrome, since it needs Local Network orBluetooth access

## AT Commands Help
TBD


## API Endpoints for streaming
TBD


## Communication CMD Structure


## Types of Steering

* Differential Steering - Steering achieved by moving wheels in opposite directions, front wheel are fixed, they cannot turn left or right
* 2WD+Differential Steering - 2 motors power left and right back motors, front wheel is unpowered and swivel. Steering acheived by moving left and right motors in opposite direction
* 4WD + Differential Steering - 4 Motors power front, back - left and right side motors. Steering acheived by moving left and right motors in opposite direction. The wheels have be be in the corners of a square, else steering is not acheived

## Motor Driver Support

* Cytron Motor Driver
* If your motor driver is not supported, please raise an issue, or create a PR
* TBD most motor drivers support PWM or direction based signals, this can be generalised. If needed we can work on this.

## SOC Support

* ESP8266 - Was seeing drop in wifi signal. Not supporting ESP8266.
* ESP32 - Both BLE and WiFI async works fine. Price effective, with Wifi asnc, same ESP32 board can be used to stream Camera, and control drivers and devices via GPIO. They seem to maintain PWM signals even when streaming camera.


## Code Editors

* Use Visual Code and plaform io extension.
* Select environment according to your board
* Visual Code with Plaform IO extension is best. 
* Arduino IDE is best for uploading small test modules. 
* Anything serious, would need multiple library support, and you will soon encounter build support for different board types. Visual Code + Platform IO makes this a breeze.

## Build Environments

* esp32cam - Async WiFi, Camera Streaming, GPIO control via http get request (working)
* nodemcu-32s-carBLE - Bluetooth (BLE), Differential Steering Car. ESP32 Board (working)
* dfrobot_romeo_esp32s3 - dfrobot board with camera - not working as expected  ( do not use )
* nodemcu-32s - placeholder ( do not use )

## Points to remember

* Most drivers need the polarity to be proper, make sure you connect the positive of power to positive on board.
* ESP32 can be powered by 5v or 3V. If available, use 5v power supply. 
* Most motor drivers, have a pinout to power the SOC unit. Which gives a standard 5v or 3v output.
* Raspberry PI is best, feature wise, but too costly for India.
* AI-Camera modules start stuttering when used with GPIO pins. Use ESP httpd server for unblocking gpio access when streaming video. Use web async for creating web server. This enables non blocking streaming of camera and control of GPIO.
* Do not use EPS32 cheap modules, they are very slow. They clock at 40 to 80mhz. When buying modules, make sure the dev board comes with USB C connector. Having external programmer is painful when prototyping. Since modern laptops have limited USB ports, having all your devices support USB C will decrease confusion with cables in future. 
* The USB C port can be used for communication with other devices too. 
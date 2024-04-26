# Project Overview - SpiCAM
Our project SpiCam is a security camera module that allows users to monitor their home/room while they are away.
SpiCam features an analog joystick for which the users can control the pan and tilt motion of the camera and maximize the field of vision, capturing every angle of the room.
It also features an automatic facial detection component where a box will be drawn around the detected face for a potential intruder. 
In addition, the users can connect to SpiCam remotely through their phones via Bluetooth and set off an alarm when necessary to scare off any intruder or suspicious personnel. 

## Technical Description
The main technical components for this project can be broken down into three parts: joystick control, facial recognition, and Bluetooth Communication
### 1. Joystick Control
The power of the joystick is connected to 3.3V, X output is connected to P1.3, and Y output is connected to P1.4 of the MSP430. As the analog joystick moves in either direction, 
the voltage output in the corresponding pins changes. We use multi-channel ADC10 of the MSP430 to continuously read the voltage values and use them to map to the PWM output of P1.6 and P2.1
that are connected to the pan servo and tilt servo of the camera module respectively. The TA0CCR1 that controls the PWM of P1.6 (pan servo) that is set proportional to the ADC value of the first channel (Joystick X direction)
and the TA1CCR1 for P2.1 PWM (tilt servo) is set proportional to the second channel ADC value (Joystick Y direction).

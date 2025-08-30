# MSP430FR5969 HC-SR04 Distance Measurement

**Author:** Fayyaz Shaikh  
**Email:** fayyaz.shaikh7862005@gmail.com  

This project demonstrates **distance measurement using the HC-SR04 ultrasonic sensor with the MSP430FR5969**. It implements a **polling-style approach** with **Timer0_A** to measure the pulse width of the echo signal, calculates the distance in centimeters, and displays it via **UART**. A **LED indicator** also turns on when a valid distance is detected.

## Features
- Polling-style HC-SR04 ultrasonic measurement
- Timer0_A used for microsecond pulse timing
- UART output for serial monitoring
- LED indication for valid distance
- Works with MSP430FR5969 LaunchPad

## Usage
1. Connect the HC-SR04 sensor to MSP430FR5969:
   - TRIGGER → P4.3
   - ECHO → P2.4
2. Connect an LED to P1.0 (optional)
3. Open a serial monitor at 115200 baud to view distance readings (Preferably COM TEST SERIAL Software)
4. Compile and flash the code using Code Composer Studio or Energia

## Notes
- Keep sensor wires short and stable for accurate readings
- Maintain a small delay between readings (~0.16s)

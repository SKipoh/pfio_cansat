# CANsat Payload Code for PlatformIO

## Requirements
- Arduino Due or Arduino Mega 2560 (Using the mega will require slight modification to the code to use SoftwareSerial)
- APC220 Radio connected to Serial2 on the Due or pins RX:10, TX:11)
- DHT22/AM2302 Temperature and Humidity Sensor
- SG90 Servo Motor
- Pmod MicroSD Card Module

## Wiring
- The Due or Mega should be powered via the "Vin" pin from a 9V battery
- The APC220 should be wired to the Ardunino Due to the Hardware Serial Port 2 (RX2 and TX2)
- DHT22 should have it's data pin (Second pin) wired to pin 7 on the Due/Mega2560
- SG90 should be wired to Pin 13 on the Due/Mega2560
- The Pmod Serial port should be connected to the SPI Header's SDA and SCL Pins

## How to use this code
1) Open this project inside of PlatformIO, check the build configuration is correct for the MCU you are using (This has only been tested on an Arduino Mega2560 and Arduino Due)
2) Check that the code builds correctly
3) Disconnect the APC220 Radio
4) Build & Upload code
5) Reconnect the APC220

# CANsat Payload Code for PlatformIO

This is part of the code needed my implementation of the ESA CANsat Competition. The code is this repo is to be used in conjunction with my other 2 repos for this project [CANsat App]() and [CANsat Ground Station]()

---

## **Hardware Requirements**

- Arduino Due or Arduino Mega 2560 (Using the mega will require slight modification to the code to use SoftwareSerial)
- APC220 Radio
- DHT22/AM2302 Temperature and Humidity Sensor
- SG90 Servo Motor
- Pmod MicroSD Card Module
- GY-89 Inertial Measurement Unit (Includes BMP180, L3GD20, LSM303 sensors)

---

## **Wiring**

![CANsat Wiring Diagram](https://raw.githubusercontent.com/gist/SKipoh/17caf35ef440576d43d99d1beaa63623/raw/426893483d9614ad7808041b3fd24c5fb44e6603/cansat-wiring.svg)

- It is advised to make up a small Power Distribution Board to help get power to each of the
components. This can be made simply and easily with a small piece of Stripboard that has at least 7 pairs of holes.
- The pMod MicroSD card holder should be powered directly from the SPI Header, although you could power it from the Power Distribution Board if you had small issues within your CANsat

---

## **How to use this code**

**THIS CODE IS DESIGNED TO BE USED IN CONJUNCTION WITH THE REST OF THE CANsat PROJECT!**

1) Open this project inside of PlatformIO, check the build configuration is correct for the MCU you are using (This has only been tested on an Arduino Mega2560 and Arduino Due)
2) Install the required libraries (Find a list of these inside the platformio.ini file)
3) Attempt to build the code
4) Disconnect the APC220 Radio & DHT22 Sensors and Power pins from the MCU
5) Plug in your MCU to your PC's USB port
6) Build & Upload code over Serial
7) Reconnect the APC220 & DHT22 Sensor and Power Pins

## Verifying CANsat Integration

Once the code is uploaded, and your wiring completed you can check the sensors are all detected by connecting the MCU to the Serial port of your PC, and opening a connection. If any of the Sensors are missing or not readable, an error message will be displayed and the MCU will halt.

For example,

``` C++
BMP180 begin() failed, check interface & I2C address

Failed to autodetect gyro type!
```

These show that even if your wires are connected, the MCU cannot read from the BMP180 temperature sensor or the L3GD20 Gyroscope. The MCU will halt. To fix this issue, disconnect any power from the satellite, then follow these steps:

1) Double check wiring, ensure that the pins are going to the correct ports (REMEMBER, for Serial, RX -> TX, and TX->RX, but with I2C, SDA -> SDA and SCL -> SCL)
2) If you are getting errors with I2C, take the board causing problems, wire it up to another MCU and run an I2C scanner, to check which addresses are actually being used. For the GY-89 Board, the typical addresses are:
    - BMP180: 0x77
    - LSM303: 0x1D
    - L3GD20: 0x6B
3) If you continue to have difficulties 

The CANsat does NOT need the MicroSD card to be inserted, and will continue to run, a message stating:

``` C++
SD card absent
```

If you have a microSD card, insert it and either press the reset button on your MCU or turn the power off and on again and the Message should not reappear in the 

If the Error persists, even with an SD inserted, check that the MicroSD card holder is not getting hot when powered on. If it is, immediately remove power, and the MicroSD card from the holder, and check your wiring to the SPI header as well as the power.

## **Useful Links**

- [Official Arduino Due Pinout](https://content.arduino.cc/assets/Pinout-Due_latest.png)
- [UNOFFICIAL Arduoino Due Pintout (with SPI Header Pinout)](http://www.robgray.com/temp/Due-pinout-WEB.png)
- [pMod Documentation (Page 4 for Pinout)](http://digilentinc.com/Pmods/Digilent-Pmod_%20Interface_Specification.pdf)
- [Adafruit DHTxx Temperature Sensor Example](https://learn.adafruit.com/dht/connecting-to-a-dhtxx-sensor)
- [Adafruit BMP085/BMP180 Sensor Example](https://learn.adafruit.com/bmp085)
- [Adafruit LSM303 Accelerometer/Magnetometer Example](https://learn.adafruit.com/lsm303-accelerometer-slash-compass-breakout/pinouts)
- [Adafruit L3GD20 Gyroscope Example](https://learn.adafruit.com/adafruit-triple-axis-gyro-breakout/arduino)
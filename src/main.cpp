#include <Arduino.h>
#include <string.h>
#include <avr/dtostrf.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Servo.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <LSM303.h>
#include <L3G.h>
#include <BMP180I2C.h>

 /*
 CANsat Release Code
 Servo Movement code that can receive commands over serial
 to move the serial to a from a start and end position
*/


// Selecting which pin to connect our DHT22 to, and instantiating it
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Instantiating an instance of Servo (MAX of 12 Servos)
Servo releaseServo;
// Defining the I2C addr of the BMP180, and creating an instance of it
#define I2C_ADDR 0x77
BMP180I2C bmp(I2C_ADDR);

// Choosing our CS pin for the SD card reader
#define CS 8

// Creating an instance of the LSM accelerometer/magnetometer
LSM303 lsm;
L3G gyro;
char DHTreport[80];
char LSMreport[300];
char L3Greport[80];
char BMPreport[160];
char completeReport[500];

// Store the servo position
int pos = 0;
// Storing received bytes
int stopBytes = 0;
// Boolean for storing whether or not to read the sensors
bool readSensors = false;

void setup() {
  // attaches the servo on pin 13 to the servo object
  releaseServo.attach(13);
  // Moving to 0
  releaseServo.write(0);
  // Opening the Serial connection to the host
  Serial.begin(9600);
  // Opening Serial Connection to the radio
  Serial2.begin(9600);
  // Opening a connection to the DHT22
  dht.begin();
  // Starting up the SPI interface
  Wire.begin();
  // Selecting our SD card reader Chip Select Pin
  pinMode(CS, OUTPUT);

  if (!bmp.begin()) {
    Serial2.println("BMP180 begin() failed, check interface & I2C address");
    while(1);
  }
  // Resetting the BMP with default values & setting it to take high-res measurements
  bmp.resetToDefaults();
  bmp.setSamplingMode(BMP180MI::MODE_UHR);

  lsm.init();
  lsm.enableDefault();

  if (!gyro.init())
  {
    Serial2.println("Failed to autodetect gyro type!");
    while (1);
  }

  if (!SD.begin(CS)) {
    Serial2.println("SD card absent");
  }

  Serial2.println("CANsat Ready to Transmit!");
}

void getTempData() {
  // Store humidity and temp readings
  float hum;
  float temp;

  // Taking our DHT readings
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  // Formatting the results into the DHT22 report as a partial JSON
  snprintf(DHTreport, sizeof(DHTreport), "'tempHumi': {'temp': %.2f, 'humidity': %.2f}",
    temp, hum);

  // example result: 'tempHumi': { 'temp': 25.43, 'humidity': 65.67 },
}

// Taking in our raw 16-bit number reading and converts to mg (milli-G)
float calcAccel(int reading) {
  float correctedReading = reading >> 4;
  correctedReading = correctedReading * 0.061;
  return correctedReading;
}

// Taking in our raw 16-bit Magnetometer reading and converts to mgauss (milli-Gauss)
float calcMag(int reading) {
  float correctedReading = reading * 0.16;
  return correctedReading;
}

// Getting the measurements from the LSM303 Accel/Magno Sensor and outputting them
char * getAccel() {
  lsm.read();

  float accel[2] = {};
  float mag[2] = {};

  // Taking our 16-bit numbers and turning them into useful output
  accel[0] = calcAccel(lsm.a.x);
  accel[1] = calcAccel(lsm.a.y);
  accel[2] = calcAccel(lsm.a.z);

  mag[0] = calcMag(lsm.m.x);
  mag[1] = calcMag(lsm.m.y);
  mag[2] = calcMag(lsm.m.z);

  snprintf(LSMreport, sizeof(LSMreport), "'accel': {'x': %.3f, 'y': %.3f, 'z': %.3f}, 'magno': {'x': %.3f, 'y': %.3f, 'z': %.3f}",
    accel[0], accel[1], accel[2],
    mag[0], mag[1], mag[2]);
}

float calcGyro(int reading) {
  reading = reading * 8.75;
  reading = reading / 1000;
  return reading;
}

char * getGyro() {
  gyro.read();

  float gyroReadings[2] = {};

  gyroReadings[0] = calcGyro(gyro.g.x);
  gyroReadings[1] = calcGyro(gyro.g.y);
  gyroReadings[2] = calcGyro(gyro.g.z);

  snprintf(L3Greport, sizeof(L3Greport), "'gyro': {'x': %.3f, 'y': %.3f, 'z': %.3f}",
    gyroReadings[0], gyroReadings[1], gyroReadings[2]);
  
}
    strcpy(gyroJson, getGyro());
    delay(100);
    getBmp();

    snprintf(completeReport, sizeof(completeReport), "{ %s, %s, %s, %s }\n", DHTreport, LSMreport, L3Greport, BMPreport);

    //Serial.println(completeReport);

    File file = SD.open("datalog.txt", FILE_WRITE);
    // Checking if the file is available
    if (file) {
      file.write(completeReport);
      file.close();
      Serial2.println("Written Report to SD");
    }

    Serial2.println(completeReport);
  }

  // Making up the time for an approx. 1 second loop
  delay(300); //Delay 1 sec.
}

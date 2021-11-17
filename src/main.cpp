#include <Arduino.h>
#include <string.h>
#include <avr/dtostrf.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>
#include <LSM303.h>
#include <BMP180I2C.h>

 /*
 CANsat Release Code
 Servo Movement code that can receive commands over serial
 to move the serial to a from a start and end position
*/


// Selecting which pin to connect our DHT22 to, and instantiating it
#define DHTPIN 7
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delaysMS;
// Instantiating an instance of Servo (MAX of 12 Servos)
Servo releaseServo;
// Defining the I2C addr of the BMP180, and creating an instance of it
#define I2C_ADDR 0x77
BMP180I2C bmp(I2C_ADDR);
// Creating an instance of the LSM accelerometer/magnetometer
LSM303 lsm;
// Creating an instance of our L3GD20 Gyro
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

// Store the servo position
int pos = 0;
// Storing received bytes
int stopBytes = 0;
// Boolean for storing whether or not to read the sensors
bool readSensors = true;
// result strings that we store globablly due to scope fuckery for all the
// formatted sensor outputs
char tempHumiJson[] = "";
char accelMagJson[] = "";
char gyroJson[] = "";
char bmpJson[] = "";
char completeData[] = "";


void setup() {
  // attaches the servo on pin 13 to the servo object
  releaseServo.attach(13);
  // Moving to 0
  releaseServo.write(0);
  // Opening the Serial connection to the host
  Serial.begin(9600);
  // Opening Serial Connection to the radio
  //Serial2.begin(9600);
  // Opening a connection to the DHT22
  dht.begin();
  // Starting up the SPI interface
  Wire.begin();

  if (!bmp.begin()) {
    Serial.println("BMP180 begin() failed, check interface & I2C address");
    while(1);
  }
  // Resetting the BMP with default values & setting it to take high-res measurements
  bmp.resetToDefaults();
  bmp.setSamplingMode(BMP180MI::MODE_UHR);

  // Starting up the LSM303 Accel/Mag sensor and enabled defaults (scale +/- 4G)
  lsm.init();
  lsm.enableDefault();

  // Setting the Gyro the auto-range mode
  gyro.enableAutoRange(true);
  // Trying to open a connection to the gyro
  if (!gyro.begin()) {
    Serial.println("NO L3GD20 Detected! Check wiring...");
    while(1);
  }
}

char * getTempData() {
  Serial.println("Getting DHT22 Data");
  // Store humidity and temp readings
  float hum;
  float temp;
  // char arrays to store our float -> str converison
  char tempArr[16];
  char humiArr[16];

  Serial.println("Init Values");

  //Read data and store it to variables hum and temp
  //hum = dht.readHumidity();
  //temp = dht.readTemperature();

  // Getting a temperature and humidity event
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  temp = event.temperature;
  hum = event.relative_humidity;

  Serial.println("Read from Sensors");

  //Print temp and humidity values to serial monitor
  strcat(tempHumiJson, "{'temp': '");
  strcat(tempHumiJson, dtostrf(temp, 4, 2, tempArr));
  strcat(tempHumiJson, "', 'humi': '");
  strcat(tempHumiJson, dtostrf(hum, 4, 2, humiArr));
  strcat(tempHumiJson, "'}, ");

  Serial.println("Phat concat in temp");

  // Returning the completed format
  // example result: {'temp': 25.74, 'humi': 51.45}
  return tempHumiJson;
}

// Taking in our raw 16-bit number reading and converts to mg (milli-G)
float calcAccel(int reading) {
  reading = reading >> 4;
  reading = reading * 0.061;
  return reading;
}

// Taking in our raw 16-bit Magnetometer reading and converts to mgauss (milli-Gauss)
float calcMag(int reading) {
  reading = reading >> 4;
  reading = reading * 0.061;
  return reading;
}

// Gets a reading from the LSM303, converts the raw output to milli-G & milli-
// gauess and returns a partial JSON format string
char * getLsm() {
  Serial.println("Getting LSM303 Data");
  // char array to store our int -> str conversions
  char tmp[80];
  // Store the Accel and Mag readings
  float accel[2] = {};
  float mag[2] = {};

  Serial.println("Init Values Done");
  // Starting a reading from the sensor
  lsm.read();

  Serial.println("Read from LSM");

  // Storing our Accelerometer results
  accel[0] = calcAccel(lsm.a.x);
  accel[0] = calcAccel(lsm.a.y);
  accel[0] = calcAccel(lsm.a.z);

  Serial.println("Calculated acceleration data");

  // formatting JSON for the accelerometer readings
  strcat(accelMagJson, "'accel': ");
  strcat(accelMagJson, "{'x': '");
  strcat(accelMagJson, dtostrf(accel[0], 5, 3, tmp));
  strcat(accelMagJson, "', 'y': '");
  strcat(accelMagJson, dtostrf(accel[1], 5, 3, tmp));
  strcat(accelMagJson, "', 'z': '");
  strcat(accelMagJson, dtostrf(accel[2], 5, 3, tmp));
  strcat(accelMagJson, "'}, ");

  Serial.println("Phat concat");

  // Storing our Accelerometer results
  mag[0] = calcMag(lsm.m.x);
  mag[0] = calcMag(lsm.m.y);
  mag[0] = calcMag(lsm.m.z);

  Serial.println("Calcualted Mag data");

  // formatting the JSON for the magnetometer readings
  strcat(accelMagJson, "'magno': ");
  strcat(accelMagJson, "{'x': '");
  strcat(accelMagJson, dtostrf(mag[0], 5, 3, tmp));
  strcat(accelMagJson, "', 'y': '");
  strcat(accelMagJson, dtostrf(mag[1], 5, 3, tmp));
  strcat(accelMagJson, "', 'z': '");
  strcat(accelMagJson, dtostrf(mag[2], 5, 3, tmp));
  strcat(accelMagJson, "'}, ");

  Serial.println("Phat string concat 2");

  return accelMagJson;
}


// Gets the x, y, z readings for the L3GD20 Gyroscope in rads/s and returns
// them as a partial JSON string
char * getGyro() {
  Serial.println("Getting L3GD20 Data");
  // char array to store our int -> str conversions
  char tmp[80];

  // Getting a new sensor event (reading)
  sensors_event_t event;
  gyro.getEvent(&event);

  // Reading our results in and formatting it into a partial JSON
  strcat(gyroJson, "'gyro': ");
  strcat(gyroJson, "{'x': '");
  strcat(gyroJson, dtostrf(event.gyro.x, 3, 2, tmp));
  strcat(gyroJson, "', 'y': '");
  strcat(gyroJson, dtostrf(event.gyro.y, 3, 2, tmp));
  strcat(gyroJson, "', 'z': '");
  strcat(gyroJson, dtostrf(event.gyro.z, 3, 2, tmp));
  strcat(gyroJson, "'}, ");

  return gyroJson;
}

char * getBmp() {
  Serial.println("Getting BMP180 Data");
  // char array to store our int -> str conversions
  char tmp[80];

  if (!bmp.measureTemperature())
	{
		Serial.println("could not start temperature measurement, is a measurement already running?");
	}

  do
	{
		delay(100);
	} while (!bmp.hasValue());

  // Making a temperature (in degC) & pressure (in Pa) measurement
  bmp.measureTemperature();
  bmp.measurePressure();

  // Reading our results and formatting into a partial JSON
  strcat(bmpJson, "'pressTemp': ");
  strcat(bmpJson, "{'temp': '");
  strcat(bmpJson, dtostrf(bmp.getTemperature(), 4, 2, tmp));
  strcat(bmpJson, "', 'press': '");
  strcat(bmpJson, dtostrf(bmp.getPressure(), 4, 2, tmp));
  strcat(bmpJson, "'}");

  return bmpJson;
}

void readInput() {
  // Checking if the Serial Port is available to use
  if (Serial.available()) {
    // Reading in any bytes and storing them
    stopBytes = Serial2.read();
    // If we receive an ASCII "a", that is the command
    // to move to the start position and then stop

    switch (stopBytes) {
      // If we receive an ASCII "a", we tell the servo the open, and release
      // the CANsat
      case 97:
        if (pos < 90) {
          for (pos = 0; pos <=90; pos++) {
            releaseServo.write(pos);
            delay(1);
          }
        }
        // Clearing stopBytes
        stopBytes = 0;
        break;
      // If we recieve an ASCII "b", we reset the servo to the start position
      case 98:
        if (stopBytes == 98) {
          releaseServo.write(0);
          // Resetting pos to 0
          pos = 0;
          // Clearing stopBytes
          stopBytes = 0;
        }
      // If we receive an ASCII "c", we toggle the sensor reading on/off
      case 99:
        readSensors = !readSensors;
        stopBytes = 0;
        break;
      }
  }
}

void loop() {
  // First thing we do is read in any input from the Ground Station and do something
  // with it
  //readInput();
  //
  // Checking if the Serial Port is available to use
  // if (Serial.available()) {
  //   // Reading in any bytes and storing them
  //   stopBytes = Serial.read();
  //   // If we receive an ASCII "a", that is the command
  //   // to move to the start position and then stop
  //   if (stopBytes == 97) {
  //     if (pos < 90) {
  //       for (pos = 0; pos <=90; pos++) {
  //         releaseServo.write(pos);
  //         delay(1);
  //       }
  //     }
  //   // Clearing stopBytes
  //   stopBytes = 0;
  //   }
  //   // If the Byte we receive is an ASCII "b", we move back to
  //   // the start position
  //   if (stopBytes == 98) {
  //     releaseServo.write(0);
  //     // Resetting pos to 0
  //     pos = 0;
  //     // Clearing stopBytes
  //     stopBytes = 0;
  //   }
  // }

  delay(200);

  Serial.println("twat");

  // // Taking measurements and returning partial JSON strings
  // char *dhtDat = getTempData();
  // char *accelMagDat = getLsm();
  // char *gyroDat = getGyro();
  // char *bmpDat = getBmp();

  String dhtDat = getTempData();
  String accelMagDat = getLsm();
  String gyroDat = getGyro();
  //String bmpDat = getBmp();

  Serial.println("megatwat");
  Serial.println(dhtDat);
  Serial.println(accelMagDat);
  Serial.println(gyroDat);
  //Serial.println(bmpDat);

  memset(tempHumiJson, 0, sizeof tempHumiJson);
  memset(accelMagDat, 0, sizeof accelMagDat);
  memset(gyroJson, 0, sizeof gyroJson);
  //memset(bmpJson, 0, sizeof bmpJson);

  // // Formatting the strings together for final transmission
  // strcat(completeData, "{ ");
  //strcat(completeData, dhtDat);
  // strcat(completeData, accelMagDat);
  // strcat(completeData, gyroDat);
  // strcat(completeData, bmpDat);
  // strcat(completeData, "}");

  // Outputting the data over the radio and local serial port
  //Serial.println(completeData);

  // Clearing our char array of results
  //completeData[0] = '\0';

  delay(2000); //Delay 2 sec.
}

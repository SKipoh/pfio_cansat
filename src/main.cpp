#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>
#include <LSM303.h>

 /*
 CANsat Release Code
 Servo Movement code that can receive commands over serial
 to move the serial to a from a start and end position
*/


//
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);


// Instantiating an instance of Servo (MAX of 12 Servos)
Servo releaseServo;

// Store the servo position
int pos = 0;
// Storing received bytes
int stopBytes = 0;
// Store humidity and temp readings
float hum;
float temp;

void setup() {
  // attaches the servo on pin 13 to the servo object
  releaseServo.attach(13);
  // Moving to 0
  releaseServo.write(0);
  // Opening the Serial connection to the host
  Serial.begin(9600);
  // Opening Serial Connection to the radio
  Serial2.begin(9600);
  dht.begin();
}

void loop() {
  // Checking if the Serial Port is available to use
  if (Serial2.available()) {
    // Reading in any bytes and storing them
    stopBytes = Serial2.read();
    // If we receive an ASCII "a", that is the command
    // to move to the start position and then stop
    if (stopBytes == 97) {
      if (pos < 90) {
        for (pos = 0; pos <=90; pos++) {
          releaseServo.write(pos);
          delay(1);
        }
      }
    // Clearing stopBytes
    stopBytes = 0;
    }
    // If the Byte we receive is an ASCII "b", we move back to
    // the start position
    if (stopBytes == 98) {
      releaseServo.write(0);
      // Resetting pos to 0
      pos = 0;
      // Clearing stopBytes
      stopBytes = 0;
    }
  }
   delay(200);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial2.print("{'temp': '");
    Serial2.print(temp);
    Serial2.print("', 'humidity': '");
    Serial2.print(hum);
    Serial2.print("'}");
    Serial2.print("\n");

    delay(799); //Delay 2 sec.
}

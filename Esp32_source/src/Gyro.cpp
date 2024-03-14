#include "gyro.h"
#include <Wire.h>
#include "I2Cdev.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ITG3200.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

ITG3200 gyro;

int16_t gx, gy, gz;

void Gyro::initAccel()
{
  /* Initialise the sensor */
  if (!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1)
      ;
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
}

// gives vallues to other cpp files
void Gyro::getAccelData(float &x, float &y, float &z)
{
  Serial.println("Accelerometer Test");
  Serial.println("");

  sensors_event_t event;
  accel.getEvent(&event);

  x = event.acceleration.x;
  y = event.acceleration.y;
  z = event.acceleration.z;
}

// prints the values
void Gyro::printAccelData()
{
  sensors_event_t event;
  accel.getEvent(&event);

  // prints vallues
  Serial.print("X: ");
  Serial.print(event.acceleration.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(event.acceleration.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(event.acceleration.z);
  Serial.print("  ");
  Serial.println("m/s^2 ");
}

void Gyro::initGyro()
{
  gyro.initialize();
}

void Gyro::getGyroData(float &x, float &y, float &z)
{
  // read raw gyro measurements from device
  gyro.getRotation(&gx, &gy, &gz);

  x = (gx * 14.375);
  y = (gy * 14.375);
  z = (gz * 14.375);
}
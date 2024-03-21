#include "gyro.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void Mpu::init()
{
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}

void Mpu::getData(float &aX, float &aY, float &aZ, float &gX, float &gY, float &gZ)
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  aX = a.acceleration.x;
  aY = a.acceleration.y;
  aZ = a.acceleration.z;
  gX = g.gyro.x;
  gY = g.gyro.y;
  gZ = g.gyro.z;
}

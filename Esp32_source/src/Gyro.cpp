#include "Gyro.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
//#include <HMC5883L.h> // HMC5883L Magnetometer Library
//#include <ITG3200.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void Gyro::initAccel(){
   /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
}

//gives vallues to other cpp files
void Gyro::getAccelData(float &x,float &y,float &z){
    Serial.println("Accelerometer Test"); Serial.println("");

    sensors_event_t event; 
    accel.getEvent(&event);
 
    x = event.acceleration.x;
    y = event.acceleration.y;
    z = event.acceleration.z;
}

//prints the values
void Gyro::printAccelData(){
    sensors_event_t event; 
    accel.getEvent(&event);

    //prints vallues
    Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
}
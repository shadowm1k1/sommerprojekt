#include <Arduino.h>
#include <WiFi.h>
#include "gyro.h"
#include "pid.h"
#include "firebase.h"

#define WIFI_SSID "Bulme-EMC"
#define WIFI_PASSWORD ""

// Gyro Object and Vallues
Gyro myGyro;
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
FirebaseDb myFirebaseDatabase;
Pid myPid;

void initWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup()
{
  Serial.begin(115200);

  // initialize Wifi
  initWiFi();

  // initialize Accelorometer
  myGyro.initAccel();
  myGyro.initGyro();
  myPid.initPid(32, 33, 1000, 2000);
  myFirebaseDatabase.init();

  Serial.println("proso init");
  delay(500);
}

void loop()
{
  // myFirebaseDatabase.upload();
  Serial.println("Accel;");
  myGyro.getAccelData(accelX, accelY, accelZ);

  myGyro.getGyroData(gyroX, gyroY, gyroZ);

  // myPid.updatePid(gyroX, gyroY, gyroZ, accelX, accelY, accelZ);
  myPid.left_prop.write(1000);
  myPid.right_prop.write(2000);
  delay(3000);
  myPid.left_prop.write(2000);
  myPid.right_prop.write(1000);
  delay(3000);
}
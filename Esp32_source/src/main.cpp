#include <Arduino.h>
#include <WiFi.h>
#include "gyro.h"
#include "pid.h"
#include "firebase.h"

#define WIFI_SSID "Bulme-EMC"
#define WIFI_PASSWORD ""

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;

Mpu myMpu;
Pid myPid;
FirebaseDb myFirebaseDatabase;

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
  myMpu.init();
  myPid.initPid(32, 33, 1000, 2000);
  // myFirebaseDatabase.init();

  Serial.println("proso init");
  delay(500);
}

void loop()
{
  // myFirebaseDatabase.upload();
  myMpu.getData(accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
  myPid.updatePid(gyroX, gyroY, gyroZ, accelX, accelY, accelZ);
}
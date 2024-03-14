#include <Arduino.h>
#include <WiFi.h>
#include "gyro.h"
#include "firebase.h"

#define WIFI_SSID "PTFWifi"
#define WIFI_PASSWORD "PtfWifiIsTheBest"

// Gyro Object and Vallues
Gyro myGyro;
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
FirebaseDb myFirebaseDatabase;

float rad_to_deg = 180 / 3.141592654;
float Acceleration_angle[2];
float Gyro_angle[2];
float Total_angle[2];

long int timepp, timePrev, elapsedTime;

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
  myFirebaseDatabase.init();

  Serial.println("proso init");
  timepp = millis();
  delay(500);
}

void loop()
{
  timePrev = timepp;
  timepp = millis();
  elapsedTime = ((timepp - timePrev) / 1000);
  // myFirebaseDatabase.upload();
  Serial.println("Accel;");
  myGyro.getAccelData(accelX, accelY, accelZ);
  Serial.print("x = ");
  Serial.print(accelX);
  Serial.print("   ");
  Serial.print("y = ");
  Serial.print(accelY);
  Serial.print("   ");
  Serial.print("z = ");
  Serial.println(accelZ);

  myGyro.getGyroData(gyroX, gyroY, gyroZ);
  Serial.println("gyro = ");
  Serial.print(gyroX);
  Serial.print("\t");
  Serial.print(gyroY);
  Serial.print("\t");
  Serial.println(gyroZ);

  Acceleration_angle[0] = atan((accelY / 256) / sqrt(pow((accelX / 256), 2) + pow((accelZ / 256), 2))) * rad_to_deg;
  Acceleration_angle[1] = atan(-1 * (accelX / 256) / sqrt(pow((accelY / 256), 2) + pow((accelZ / 256), 2))) * rad_to_deg;
  Gyro_angle[0] = gyroX / 14.375;
  Gyro_angle[1] = gyroY / 14.375;
  Total_angle[0] = 0.98 * (Total_angle[0] + Gyro_angle[0]*elapsedTime) + 0.02*Acceleration_angle[0];
  Total_angle[1] = 0.98 * (Total_angle[1] + Gyro_angle[1]*elapsedTime) + 0.02*Acceleration_angle[1];

float rad_to_deg = 180/3.141592654;

  Serial.println("accel");
  Serial.print(Acceleration_angle[0]);Serial.print("\t"); Serial.println(Acceleration_angle[1]);

  Serial.println("totalAngel");
  Serial.print(Total_angle[0]);Serial.print("\t"); Serial.println(Total_angle[1]);
  delay(500);
}
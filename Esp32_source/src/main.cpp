#include <Arduino.h>
#include <WiFi.h>
#include "gyro.h"
#include "pid.h"
#include "firebase.h"
#include "Tasker.h"

#define SOUND_SPEED 0.034
#define MIN_DISTANCE 7.0

#define WIFI_SSID "A1-C33D8E"
#define WIFI_PASSWORD "Bajrektarevic00%"

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;

const int trigPin = 18;
const int echoPin = 19;
const int trigPin2 = 5;
const int echoPin2 = 17;

// define sound speed in cm/uS

long duration;
float distanceLeft;
float distanceRight;
float distance;

Mpu myMpu;
Pid myPid;
FirebaseDb myFirebaseDatabase;
Tasker tasker;
int speed = 1000;

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

// ultraschall func
float getDistance(const int trigPin, const int echoPin);
void setDistances();
void checkObsticles();

// MPU func
void checkMPUPID();

void setup()
{

  Serial.begin(115200);

  // initialize Wifi
  initWiFi();

  // initialize Accelorometer
  myMpu.init();
  myPid.initPid(32, 33, 1000, 2000);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT);
  //myFirebaseDatabase.init();
  tasker.setInterval(checkObsticles, 500); // check for obsticles every 500ms | using tasker to multithread
  tasker.setInterval(checkMPUPID, 1);
}

void loop()
{
  tasker.loop();
  //myFirebaseDatabase.upload();
  //delay(3000);
  Serial.println(myPid.error);
}

void setDistances()
{
  distanceRight = getDistance(trigPin, echoPin);
  distanceLeft = getDistance(trigPin2, echoPin2);
}
float getDistance(const int trigPin, const int echoPin)
{

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distance = duration * SOUND_SPEED / 2;
  return distance;
}
void checkObsticles()
{
  setDistances();

  if (distanceLeft < MIN_DISTANCE && distanceRight > MIN_DISTANCE * 3)
  {
    myPid.desired_angle = 20;
  }
  else if (distanceRight < MIN_DISTANCE && distanceLeft > MIN_DISTANCE * 3)
  {
    myPid.desired_angle = -20;
  }
  else
  {
    myPid.desired_angle = 0;
  }
}

void checkMPUPID()
{
  myMpu.getData(accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
  myPid.updatePid(gyroX, gyroY, gyroZ, accelX, accelY, accelZ);
}
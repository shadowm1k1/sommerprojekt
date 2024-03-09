#include <Arduino.h>
#include <WiFi.h>
#include "GY_85.h"
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define API_KEY "AIzaSyC41z5UgfqDsD5ehmk9ouIxQj1DHagjF68"
#define DATABASE_URL "https://dronesimproject2024-default-rtdb.europe-west1.firebasedatabase.app/"

#define WIFI_SSID "Bulme-EMC"
#define WIFI_PASSWORD ""

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// create GYRO OBJECT
GY_85 GY85;

String uid;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

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

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  GY85.init();
  Serial.println("proso init");
  delay(500);

  //
  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "")
  {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.print(uid);
}

void loop()
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0, 100)))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  int *accelerometerReadings = GY85.readFromAccelerometer();
  int ax = GY85.accelerometer_x(accelerometerReadings);
  int ay = GY85.accelerometer_y(accelerometerReadings);
  int az = GY85.accelerometer_z(accelerometerReadings);

  int *compassReadings = GY85.readFromCompass();
  int cx = GY85.compass_x(compassReadings);
  int cy = GY85.compass_y(compassReadings);
  int cz = GY85.compass_z(compassReadings);

  float *gyroReadings = GY85.readGyro();
  float gx = GY85.gyro_x(gyroReadings);
  float gy = GY85.gyro_y(gyroReadings);
  float gz = GY85.gyro_z(gyroReadings);
  float gt = GY85.temp(gyroReadings);

  // Log it to serial port
  Serial.print("accelerometer");
  Serial.print(" x:");
  Serial.print(ax);
  Serial.print(" y:");
  Serial.print(ay);
  Serial.print(" z:");
  Serial.print(az);

  Serial.print("\t compass");
  Serial.print(" x:");
  Serial.print(cx);
  Serial.print(" y:");
  Serial.print(cy);
  Serial.print(" z:");
  Serial.print(cz);

  Serial.print("\t  gyro");
  Serial.print(" x:");
  Serial.print(gx);
  Serial.print(" y:");
  Serial.print(gy);
  Serial.print(" z:");
  Serial.print(gz);
  Serial.print("\t gyro temp:");
  Serial.println(gt);

  // Make delay between readings
  delay(100);
}
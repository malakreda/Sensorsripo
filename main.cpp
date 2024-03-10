#include <Arduino.h>
#include "MQ135.h"
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define WIFI_SSID "MalakReda"
#define WIFI_PASSWORD "malak2006"
#define API_KEY "AIzaSyAd_kHbHSdHgFpamrEtDh-dKxAvsGFcqn8"
#define DATABASE_URL "https://app3-33a17-default-rtdb.europe-west1.firebasedatabase.app/" 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


const float max_volts = 3.3;
const float max_analog_steps = 4095;
#define PIN_MQ135 36
MQ135 mq135_sensor = MQ135(PIN_MQ135);
void setup() {
  Serial.begin(9600);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);

  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


}

void loop() {

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  int a0_read = analogRead(34);
  Serial.print(" NO2: ");
  float No=a0_read * (max_volts / max_analog_steps);
  Serial.println(No);
  float ppm = mq135_sensor.getPPM();
      Serial.println("The amount of CO2 (in PPM): ");
    Serial.println(ppm);

  delay(2000);
 

  if (Firebase.RTDB.setInt(&fbdo, "/no2", No)){
      Serial.println("PASSED");
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setInt(&fbdo, "/co2", ppm)){
      Serial.println("PASSED");
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }    
    
  }

}
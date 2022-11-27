#include <FirebaseESP32.h>

//Firebase Library
//#include <Firebase_ESP_Client.h>
#include <FirebaseESP32.h>
// DHT library
#include <TinyGPS++.h>
#include <SoftwareSerial.h> 
              //Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Redmi9"
#define WIFI_PASSWORD "123456780"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDPQqiBpIUcA3gbBHSqfFGh6c0kTcKTYXo"

// Insert RTDB URLefine the RTDB URL */
#define FIREBASE_HOST "gpssurya-default-rtdb.firebaseio.com"                          // the project name address from firebase id
#define FIREBASE_AUTH "BYIFh0dDjvG1rSTHVyXsMiUVzddq2B4Nbk1xVBaR"            // the secret key generated from firebase
#define DATABASE_URL "https://gpssurya-default-rtdb.firebaseio.com/"

static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

String status_relay1 = "";
#define pinRELAY1 2

String status_relay2 = "";
#define pinRELAY2 15


//Define Firebase Data object
FirebaseData fbdo;
FirebaseData firebaseData;//FirebaseData firebasedata;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup() {
//pinMode(Led, OUTPUT);
pinMode(pinRELAY1, OUTPUT); 
pinMode(pinRELAY2, OUTPUT);
//pinMode(Led,OUTPUT); 
    //pinMode(Dev_3,OUTPUT);//initialize the Device OUTPUT  
   // pinMode(Dev_4,OUTPUT);//initialize the Device OUTPUT
  
     
Serial.begin(9600);
ss.begin(GPSBaud);
  //pin Relay
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
void RLY1() {
  if (Firebase.getString(firebaseData, "relay1")) { // "relay1" adalah tag yang digunakan untuk mengenali relay antara ESP dengan Aplikasi 
    if (firebaseData.dataType() == "string") {
      status_relay1 = firebaseData.stringData();
      Serial.print("Stat Relay 1:"); Serial.print(status_relay1); Serial.print("  ");
      if (status_relay1 == "1") {
        digitalWrite (pinRELAY1, HIGH);
        Serial.println("RELAY ON");
        //digitalWrite (Led, HIGH);
      } else if (status_relay1 == "0") {
        digitalWrite (pinRELAY1, LOW);
         //digitalWrite(Led,LOW);
        Serial.println("RELAY OFF");
        //digitalWrite (Led, LOW);
      }
    }
  }
}
void RLY2() {
  if (Firebase.getString(firebaseData, "relay2")) { // "relay1" adalah tag yang digunakan untuk mengenali relay antara ESP dengan Aplikasi 
    if (firebaseData.dataType() == "string") {
      status_relay2 = firebaseData.stringData();
      Serial.print("Stat Relay 2:"); Serial.print(status_relay2); Serial.print("  ");
      if (status_relay2 == "1") {
        digitalWrite (pinRELAY2, LOW);
        Serial.println("RELAY OFF");
       // digitalWrite (Led,LOW );
      } else if (status_relay2 == "0") {
        digitalWrite (pinRELAY2, HIGH);
        Serial.println("RELAY OFF");
        //digitalWrite (Led, LOW);
      }
    }
  }
}
void loop() {
 RLY1();
 RLY2();
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      Serial.print("Latitude= ");
      Serial.print(gps.location.lat());
      Serial.print(" Longitude= ");
      Serial.println(gps.location.lng());
      delay(1500);
    }
 if(gps.location.isValid()) {
  float lat =(gps.location.lat());
  float lng=(gps.location.lng());
  String latitude=(String)lat;
  String longitude=(String)lng;
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1500 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // menulis data suhu dari DHT11 ke firebase
    if (Firebase.RTDB.setString(&fbdo, "/GPS/f_latitude", latitude)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      delay(2000);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
     // menulis data kelembaban dari DHT11 ke firebase
    if (Firebase.RTDB.setString(&fbdo, "/GPS/f_longitude",longitude )) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      delay(2000);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
  }
}
}
}

//Author: Dhrubo Roy Partho
//Project Name: Vehicle Tracking
//Date: 15/01/2024
//Version: 1.0v


#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h> 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266Firebase.h>

//Firebase realtime database credential
#define ref_url "https://bus-tracking-7f6f8-default-rtdb.firebaseio.com"

//Firebase connection
Firebase firebase(ref_url);

//Device ID
String device_id = "BUS_01";

//GPS Serial Rx, Tx
#define Rx 4
#define Tx 5

//GPS object and Serial
TinyGPSPlus gps;
SoftwareSerial gps_serial(Rx, Tx);

//GPS value store variables
float Latitude, Longitude, Speed;
String lat_str, long_str, speed_str;

//Wifi connection credentials
const char* ssid = "5Thfloor";
const char* pass = "12345600";

//URL for send data to server
String base_url = "http://192.168.0.105/input.php";    //Base server
String url = base_url;

//HTTP and WiFiClient objects
WiFiClient client;
HTTPClient http;

//GPS, WIFI error counter like time out
int error_ct_gps = 0, error_ct_wifi = 0,  error_ct_both = 0;

//Database data saving path in firebase
String path = "locations/" + device_id;

void setup() {
  //Monitor serial communication start
  Serial.begin(9600);

  //GPS serial communication start
  gps_serial.begin(9600);

  //Wifi connection
  connectWifi();

  //Data sending
//  sendReq();
}

void loop() {
  //Checking Wifi and GPS connection both ok or not
  while(WiFi.status() == WL_CONNECTED && gps_serial.available() > 0){
    error_ct_gps = 0;
    error_ct_wifi = 0;
    error_ct_both = 0;
    if(gps.encode(gps_serial.read())){
      if(gps.location.isValid() || gps.speed.isValid()){
        Latitude = gps.location.lat(); 
        lat_str = String(Latitude, 6);
        Longitude = gps.location.lng();
        long_str = String(Longitude, 6);
        Speed = gps.speed.kmph();
        int sd = int(Speed);
        speed_str = String(sd);

        Serial.print("Latitude: "); Serial.println(lat_str);
        Serial.print("Longitude: "); Serial.println(long_str);
        Serial.print("Speed: "); Serial.print(Speed); Serial.println(" kmph");
        Serial.println("-------------------------------");

        // sendReq(lat_str, long_str);
        send_to_firebase(lat_str, long_str, speed_str);
        // smartDelay(100);
        delay(50);
      
      }  
    }
  }

  //Wifi error
  if(WiFi.status() != WL_CONNECTED){
    error_ct_wifi++;
    if(error_ct_wifi > 1000){
      Serial.println("Wifi connection error. Tring to connect");
      connectWifi();
    }
  }
  //GPS error
  else if(!gps_serial.available()){
    error_ct_gps++;
    if(error_ct_gps > 1000){
      Serial.println("Satelite connection unstable. Tring to connect.");
      while(!gps_serial.available()){
        Serial.print(".");
        delay(50);
      }
    }
  }
  //Wifi GPS both error
  else{
    error_ct_both++;
    if(error_ct_both > 1000){
      Serial.println("Wifi and GPS both connection are unstable. Please check internet connection and sky.");
      delay(50);
    }
  }
}


//Wifi connection function
void connectWifi()
{
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, pass);
  Serial.println("Connecting to wifi");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to: "); Serial.println(ssid);
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
}



//Data sender function to webserver
void sendReq(String latitude, String longitude)
{
  url += "?id=" + device_id + "&lat=" + latitude + "&long=" + longitude;
  
  http.begin(client, url);

  int httpCode = http.GET();

  String payload = http.getString();

  //Meta information
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  Serial.print("URL: "); Serial.println(url);
//  Serial.print("DATA: "); Serial.println(postdata);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload: "); Serial.println(payload);  
  url = base_url;
}


//Data sender funtion for firebase
void send_to_firebase(String latitude, String longitude, String speed)
{
  String location_str = latitude + ", " + longitude + ", " + speed;
  firebase.pushString(path, location_str);
  return;
}


//Smart delay
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gps_serial.available())
      gps.encode(gps_serial.read());
  } while (millis() - start < ms);
}

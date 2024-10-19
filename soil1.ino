#include <WiFi.h>//library for connectivity
#include <HTTPClient.h>//library for http request

// Moisture sensor pin
const int sensor = 35;// intializing soil moisture
int value;
//int pump = ;

// ThingSpeak credentials
const String apiKey = "S125DDOT56065THT";//thing speak api key
const char* ssid = "Redmi 9A";
const char* password = "jaiverma125";
const char* server = "http://api.thingspeak.com/update";

void setup() {
  Serial.begin(115200);//start serial communication
  delay(10);
  
  pinMode(sensor, INPUT);
//  pinMode(pump, OUTPUT);
//  digitalWrite(pump,LOW);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {//check the connection
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  value = analogRead(sensor);
  value = constrain(value, 0, 1023); // keep the range 0 to 1023
  //value = map(value, 0, 1023, 100, 0); // Optional mapping to 0-100

  if (WiFi.status() == WL_CONNECTED) {//check wifi connection before sending data
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(value);
    
    http.begin(url);
    int httpCode = http.GET(); // Use GET method as the URL contains the POST data
    
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error on HTTP request");
    }
    
    http.end();
  } else {
    Serial.println("Error: Not connected to WiFi");
  }
    //if (value> 800){
    //  digitalWrite(pump,HIGH);
    //}
    //else {
    //  digitalWrite(pump,LOW);
    //}

  Serial.println("Soil moisture is: " + String(value));
  Serial.println("Sending data to ThingSpeak");
  Serial.println("Waiting 20 secs");

  // ThingSpeak needs at least a 15 sec delay between updates
  delay(5000);
}

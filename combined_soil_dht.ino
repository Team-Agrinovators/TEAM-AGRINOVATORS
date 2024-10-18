#include <DHT.h>
#include <DHT_U.h>

#include <WiFi.h>
#include <HTTPClient.h>

// Moisture sensor pin
const int sensor = 35;
const int DHTPIN = 18;
int value;


#define DHTTYPE DHT22 


DHT dht(DHTPIN, DHTTYPE);


const String apiKey = "X8ET4LQDAMTNVYJC";
const char* ssid = "Redmi 9A";
const char* password = "jaiverma125";
const char* server = "http://api.thingspeak.com/update";

void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(sensor, INPUT);
  
  
  dht.begin();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  value = analogRead(sensor);
  //value = constrain(value, 0, 1023); 
  //value = map(value, 0, 1023, 100, 0); 

  float t = dht.readTemperature(); 
  float h = dht.readHumidity();    
  
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(t) + "&field2=" + String(h) + "&field3=" + String(value);
    
    http.begin(url);
    int httpCode = http.GET(); 
    
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

  Serial.println("Soil moisture is: " + String(value));
  Serial.println("Temperature is: " + String(t));
  Serial.println("Humidity is: " + String(h));
  Serial.println("Sending data to ThingSpeak");
  Serial.println("Waiting 20 secs");

  
  delay(20000);
}
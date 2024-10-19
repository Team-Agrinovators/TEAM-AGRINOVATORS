#include <DHT.h>//library for dht sensor
#include <DHT_U.h>//additional dht sensor sensor library
#include <WiFi.h>//library for wifi connectivity
#include <HTTPClient.h>//library for making http request
#include <ArduinoJson.h>//library for json data
#include <U8g2lib.h>//library for oled dispaly
#include <Wire.h>//i2c integration

// Moisture sensor pin
const int sensor = 35;
const int DHTPIN = 18;
const int pump = 14; 
int value;

#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);


const String apiKeyThing = "X8ET4LQDAMTNVYJC";
const char* ssid = "Redmi 9A"; // Enter your WiFi name 
const char* password = "jaiverma125"; // Enter your WiFi password
const char* serverThing = "http://api.thingspeak.com/update";

String ApiKey = "b8ebae1a8ddb81e843241de8a3325407"; // Get your API key from OpenWeather

// METHOD 2 YOUR CITY
String URL = "https://api.openweathermap.org/data/2.5/weather?q="; // URL for your city
String your_city = "Kurukshetra"; // Enter your city name

int LED = 5;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(sensor, INPUT);
  pinMode(pump,OUTPUT);
  digitalWrite(pump,LOW);
  Serial.begin(115200); // Initialize Serial communication
  //u8g2.begin();
  dht.begin();
  WiFi.begin(ssid, password);//connect to wifi

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print("."); // Display Error message
  }
  Serial.println("");
  Serial.println("WiFi connected");

  digitalWrite(LED, HIGH);//turn on led to show succesfull connection
  delay(1000);
}

void loop() {
  value = analogRead(sensor);
  float t = dht.readTemperature(); 
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {//check if dht readings are correct
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {//check wifi connection before sending data to thingspeak
    HTTPClient http;

    String url = String(serverThing) + "?api_key=" + apiKeyThing + "&field1=" + String(t) + "&field2=" + String(h) + "&field3=" + String(value);
    
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

  //delay(20000);

  // Weather Data Fetch
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(URL + your_city + "&appid=" + ApiKey + "&units=metric"); // URL for your city

    int httpCode = http.GET();

    if (httpCode > 0) {
      String JSON_Data = http.getString();
      Serial.println("Received JSON Data:");
      Serial.println(JSON_Data);

      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      // Storing information received from the JSON file in the variables
      JsonObject obj = doc.as<JsonObject>();
      const char* description = obj["weather"][0]["main"].as<const char*>();
      const float temperature = obj["main"]["temp"].as<float>();
      const float feel = obj["main"]["feels_like"].as<float>();
      const float wind = obj["wind"]["speed"].as<float>();
      const char* city = obj["name"].as<const char*>();
      const char* country = obj["sys"]["country"].as<const char*>();
      const float humidity = obj["main"]["humidity"].as<float>();
      // Converting the information into String data type
      String descriptionStr = String(description);
      String temperatureStr = String(temperature, 1);  // 1 decimal place
      String feelStr = String(feel, 1);         // 1 decimal place
      String windStr = String(wind, 1);         // 1 decimal place
      String cityStr = String(city);
      String countryStr = String(country);
      String humidityStr = String(humidity, 0);

      // Print the weather data to Serial Monitor
      Serial.println("Weather Data:");
      Serial.print("City: ");
      Serial.println(cityStr);
      Serial.print("Country: ");
      Serial.println(countryStr);
      Serial.print("Temperature: ");
      Serial.println(temperatureStr + " °C");
      Serial.print("Feels Like: ");
      Serial.println(feelStr + " °C");
      Serial.print("Humidity: ");
      Serial.println(humidityStr + " %");
      Serial.print("Wind Speed: ");
      Serial.println(windStr + " m/s");
      Serial.print("Description: ");
      Serial.println(descriptionStr);


      //blink();
      delay(20000);  // Delay for 30 seconds
    } else {
      //error();
      Serial.println("HTTP GET failed. Error code: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
  if (value > 2700){
    digitalWrite(pump,HIGH);
  }
  else{
    digitalWrite(pump,LOW);
  }

}



#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Wire.h>

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
String URL = "https://api.openweathermap.org/data/2.5/weather?q="; // URL for your city
String your_city = "Gwalior"; // Enter your city name

// Twilio credentials
static const char *account_sid = "Twilio SSID"; // Your Account SID (HIDDEN FOR SECURITY REASONS)
static const char *auth_token = "******************"; // Your Auth Token (HIDDEN FOR SECURITY REASONS)
static const char *from_number = "+18646333398"; // Your Twilio phone number
static const char *to_number = "+918506958136"; // Recipient's phone number

int LED = 5;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(sensor, INPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  Serial.begin(115200); // Initialize Serial communication
  dht.begin();
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print("."); // Display Error message
  }
  Serial.println("");
  Serial.println("WiFi connected");

  digitalWrite(LED, HIGH);
  delay(1000);
}

// SMS (MQTT PROTOCOL)
void sendSMS(String message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://api.twilio.com/2010-04-01/Accounts/";
        url += account_sid;
        url += "/Messages.json";

        http.begin(url);
        http.setAuthorization(account_sid, auth_token);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String payload = "From=" + String(from_number) +
                         "&To=" + String(to_number) +
                         "&Body=" + message;

        int httpResponseCode = http.POST(payload);
        
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Response: " + response);
            Serial.println("Sent message successfully!");
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close connection
    } else {
        Serial.println("WiFi not connected!");
    }
}

void loop() {
  value = analogRead(sensor);       // Soil Moisture Sensor Value
  float t = dht.readTemperature();  // Temperature Value
  float h = dht.readHumidity();     // Humidity Value
  /*if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }*/

  // WIFI CONNECTIVITY PROTOCOL
  if (WiFi.status() == WL_CONNECTED) {
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

  // Print all sensor reading to serial monitor
  Serial.println("Soil moisture is: " + String(value));
  Serial.println("Temperature is: " + String(t));
  Serial.println("Humidity is: " + String(h));
  Serial.println("Sending data to ThingSpeak");
  Serial.println("Waiting 20 secs");

//pump
/*if (value > 2700) {
    digitalWrite(pump, HIGH);
  } else {
    digitalWrite(pump, LOW);
  }*/

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

      // Check if rain is expected
      bool rainExpected = obj.containsKey("rain");

      // Create a message with the soil moisture reading and rain alerts..
      String moistureMessage = "Soil Moisture Level: " + String(value) + "\n" +
                               "मिट्टी की नमी का स्तर: " + String(value);

      if (value > 2700) {
          digitalWrite(pump, HIGH);
          moistureMessage += "\nPump is working. No rain expected!";
          moistureMessage += "\nपंप काम कर रहा है! बारिश की कोई उम्मीद नहीं है!";
      } else {
          digitalWrite(pump, LOW);
          moistureMessage += "\nPump is not working";
          moistureMessage += "\nपंप काम नहीं कर रहा है";
      }

      // Send SMS with the soil moisture reading and rain alert
      sendSMS(moistureMessage);

      // Print the weather data to Serial Monitor
      Serial.println("Weather Data:");
      Serial.print("City: ");
      Serial.println(city);
      Serial.print("Country: ");
      Serial.println(country);
      Serial.print("Temperature: ");
      Serial.println(String(temperature) + " °C");  // Convert temperature to String
      Serial.print("Feels Like: ");
      Serial.println(String(feel) + " °C");          // Convert feel to String
      Serial.print("Humidity: ");
      Serial.println(String(humidity) + " %");       // Convert humidity to String
      Serial.print("Wind Speed: ");
      Serial.println(String(wind) + " m/s");         // Convert wind speed to String

      delay(20000);  // Delay for 20 seconds
    } else {
      Serial.println("HTTP GET failed. Error code: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }

  
}

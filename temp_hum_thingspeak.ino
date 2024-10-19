#include <WiFi.h> //wifi connectivity
#include <HTTPClient.h>//htpp request
#include "DHT.h"//dht sensor  integration

// DHT22 is defined as DHT22 instead of DHT11
#define DHTPIN 18
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Redmi 9A";
const char* password = "jaiverma125";
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Write API Key provided by ThingSpeak
String apiKey = "YCLPO2MU7N1HHEC5";

void setup() {
  Serial.begin(115200);//start serial commmunication
  WiFi.begin(ssid, password);//connect to wifi
  dht.begin();//dht sensor intialization

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {//check connection
    WiFiClient client;
    HTTPClient http;
    delay(10000); // wait for 10 seconds

    // Read temperature and humidity
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Check if reading failed
    if (isnan(t) || isnan(h)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(t) + "&field2=" + String(h);
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);//print response code

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

#include <DHT.h>

// Define the pin where the DHT22 is connected
#define DHTPIN 4      // GPIO4 (Change this to the appropriate pin)

// Define the type of sensor used
#define DHTTYPE DHT22   // DHT22 or DHT11

// Create an instance of the DHT class
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Start serial communication
  Serial.begin(115200);
  
  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements
  delay(2000);
  
  // Read humidity
  float humidity = dht.readHumidity();
  
  // Read temperature in Celsius
  float temperature = dht.readTemperature();
  
  // Print the results to the Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
}

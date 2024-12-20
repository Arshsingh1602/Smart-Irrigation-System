#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

// Pin Definitions
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define DHTPIN 4
#define DHTTYPE DHT22
#define SOIL_MOISTURE_PIN 32

// DHT22 Instance
DHT dht(DHTPIN, DHTTYPE);

// LoRa ID
const int sensorId = 1;  // Replace with the appropriate sensor ID
const float bulkDensity = 0.4; // Bulk density of clayey soil

// Soil moisture sensor calibration values
const int AIR_RAW = 4095;             // Sensor reading in air
const int FIELD_CAPACITY_RAW = 1000;  // Sensor reading at field capacity
const float FIELD_CAPACITY = 0.52;    // Field capacity as a fraction

// Function to calculate GWC
float calculateGWC(int rawValue) {
  if (rawValue <= FIELD_CAPACITY_RAW) {
    return 1.0; // Fully saturated
  } else {
    return (float)(AIR_RAW - rawValue) / (AIR_RAW - FIELD_CAPACITY_RAW);
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LoRa module
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {  // Replace 433E6 with your frequency
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSpreadingFactor(12); // Higher spreading factor for robust transmission
  LoRa.setCodingRate4(5);      // Stronger error correction
  Serial.println("LoRa initialized successfully!");

  pinMode(SOIL_MOISTURE_PIN, INPUT);
}

void loop() {
  // Read DHT22 data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read soil moisture sensor data
  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  float soilMoistureGWC = calculateGWC(soilMoistureRaw);
  float soilMoistureVMC = soilMoistureGWC * bulkDensity;
  
  // Adjusted calculation for percentage of field capacity
  float soilMoistureFC = (soilMoistureVMC / FIELD_CAPACITY) * 100;

  // Correct the calibration to make field capacity align with 100% in real-life testing
  if (soilMoistureFC > 100) {
    soilMoistureFC = 100; // Clamp value to maximum of 100%
  }

  // Check if DHT22 values are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Format data into a string
  String dataPacket = "Sensor data of T" + String(sensorId) +
                      ". Temperature: " + String(temperature, 1) + "C, " +
                      "Humidity: " + String(humidity, 1) + "%, " +
                      "Soil Moisture (% FC): " + String(soilMoistureFC, 2) + "%";

  // Send data via LoRa
  LoRa.beginPacket();
  LoRa.print(dataPacket);
  LoRa.endPacket();

  // Print the data to the Serial Monitor
  Serial.println("Data sent: " + dataPacket);

  // Delay to avoid flooding the LoRa network
  delay(10000);  // Short delay for real-time updates
}

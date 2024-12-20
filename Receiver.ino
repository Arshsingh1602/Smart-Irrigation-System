#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// LoRa pins
#define SS 5    // Chip Select
#define RST 14  // Reset
#define DIO0 26 // IRQ

// Relay pins
#define RELAY1_PIN 25
#define RELAY2_PIN 27

// Firebase configuration
#define WIFI_SSID "Arsh"
#define WIFI_PASSWORD "arsh2005"
#define FIREBASE_API_KEY "AIzaSyDOGffcwZ9uZHlI_kZgBRz2QE7cDf2TnAU"
#define DATABASE_URL "https://truptasinchan01-84563-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

// Setup WiFi and Firebase
void setupWiFiAndFirebase() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");

  // Firebase configuration
  config.api_key = FIREBASE_API_KEY;
  auth.user.email = "arsharya1604@gmail.com";
  auth.user.password = "1234567890";
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  if (!Firebase.ready()) {
    Serial.println("Firebase initialization failed.");
    Serial.print("Reason: ");
    Serial.println(config.signer.tokens.error.message.c_str());
  } else {
    Serial.println("Firebase connected!");
  }
}

// Setup function
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Transceiver with Firebase Integration");

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSpreadingFactor(12);
  LoRa.setCodingRate4(5);
  Serial.println("LoRa initialized.");

  // Initialize WiFi and Firebase
  setupWiFiAndFirebase();

  // Initialize relay pins
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
}

// Function to validate and parse the LoRa message
bool validateAndParseMessage(String message, String &sensorId, String &temperature, String &humidity, String &soilMoisture) {
  if (!message.startsWith("Sensor data of T") || message.indexOf(". Temperature:") == -1 ||
      message.indexOf("Humidity:") == -1 || message.indexOf("Soil Moisture (% of FC):") == -1) {
    return false; // Invalid format
  }

  // Extract Sensor ID
  int idStart = 15; // Start after "Sensor data of T"
  int idEnd = message.indexOf('.');
  sensorId = message.substring(idStart, idEnd);

  // Extract Temperature
  int tempStart = message.indexOf("Temperature: ") + 13;
  int tempEnd = message.indexOf("C,");
  temperature = message.substring(tempStart, tempEnd);

  // Extract Humidity
  int humStart = message.indexOf("Humidity: ") + 10;
  int humEnd = message.indexOf("%, Soil");
  humidity = message.substring(humStart, humEnd);

  // Extract Soil Moisture
  int soilStart = message.indexOf("Soil Moisture (% of FC): ") + 25;
  soilMoisture = message.substring(soilStart, message.indexOf("%", soilStart));

  return true;
}

// Function to upload data to Firebase
void uploadToFirebase(String sensorId, String temperature, String humidity, String soilMoisture) {
  
  if (!sensorId.startsWith("T")) {
    sensorId = "T" + sensorId;
  }
  
  String basePath = "/transmitter/" + sensorId;

  if (Firebase.RTDB.setString(&fbdo, basePath + "/temperature", temperature)) {
    Serial.println("Temperature uploaded.");
  } else {
    Serial.println("Failed to upload temperature: " + fbdo.errorReason());
  }

  if (Firebase.RTDB.setString(&fbdo, basePath + "/humidity", humidity)) {
    Serial.println("Humidity uploaded.");
  } else {
    Serial.println("Failed to upload humidity: " + fbdo.errorReason());
  }

  if (Firebase.RTDB.setString(&fbdo, basePath + "/soil_moisture", soilMoisture)) {
    Serial.println("Soil moisture uploaded.");
  } else {
    Serial.println("Failed to upload soil moisture: " + fbdo.errorReason());
  }
}

// Function to check and control irrigation valves based on Firebase data
void checkAndControlValves() {
  for (int i = 1; i <= 2; i++) {
    String valvePath = "/valves/T" + String(i) + "/state";
    if (Firebase.RTDB.getBool(&fbdo, valvePath)) {
      if (fbdo.dataType() == "boolean") {
        bool valveState = fbdo.boolData();
        Serial.print("Valve T");
        Serial.print(i);
        Serial.print(" state: ");
        Serial.println(valveState ? "ON" : "OFF");

        // Control relays
        digitalWrite(i == 1 ? RELAY1_PIN : RELAY2_PIN, valveState ? HIGH : LOW);
      }
    } else {
      Serial.println("Failed to retrieve valve state: " + fbdo.errorReason());
    }
  }
}

// Loop function
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedMessage = "";
    while (LoRa.available()) {
      receivedMessage += (char)LoRa.read();
    }

    Serial.print("Received message: ");
    Serial.println(receivedMessage);

    String sensorId, temperature, humidity, soilMoisture;
    if (validateAndParseMessage(receivedMessage, sensorId, temperature, humidity, soilMoisture)) {
      Serial.println("Parsed Data:");
      Serial.println("Sensor ID: " + sensorId);
      Serial.println("Temperature: " + temperature);
      Serial.println("Humidity: " + humidity);
      Serial.println("Soil Moisture: " + soilMoisture);

      uploadToFirebase(sensorId, temperature, humidity, soilMoisture);
    } else {
      Serial.println("Invalid message format.");
    }
  }

  checkAndControlValves();
  delay(2000);
}

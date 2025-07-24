# Smart Irrigation System using LoRa and ESP32

This project is a transmitter-receiver duo using the LoRa communication protocol that helps us to revolutionize the agriculture industry by advancing the Smart Irrigation System. The transmitter acts as the node that transmits the data, and the receiver acts as the central node to receive that data and communicate it further, and process it.

Features of the project include: 
  1. Soil Moisture Detection
  2. Temperature and Humidity Detection
  3. Transmission (Long Range(upto 12-15km in rural areas))
  4. External parameters fetching, calculations, and predictions based on ML models
  5. Response to outcomes makes it a two-way system without human intervention.

| Component                 | Quantity | Description / Link         |
|---------------------------|----------|-----------------------------|
| ESP32 Dev Board           | 2        | WiFi + Bluetooth enabled    |
| SX1278 LoRa Module        | 2        | 433MHz                     |
| Soil Moisture Sensor      | 1        | Capacitive type            |
| MCP9808 Temp Sensor/DHT11 | 1        | Temperature sensor         |
| Water Valve (12V)         | 1        | Controlled via relay       |
| Relay Module              | 1        | For switching valve        |
| Power Supply (12V, 5V)    | 1 each   | For sensors and motors     |

To understand how the project works, we must understand some things first:

Soil Moisture Content: The amount of water present in the soil, usually measured as a percentage. It can be volumetric (by volume) or gravimetric (by weight).
Field Capacity (FC): The maximum moisture level that soil retains after excess water has drained. It represents the ideal moisture for plant growth.
Wilting Point (WP): The minimum moisture level at which plants begin to wilt due to lack of available water.
Permanent Wilting Point (PWP): A critical level where plants can no longer recover, even if water is reintroduced.
Available Water Content (AWC): The difference between field capacity and wilting point. It indicates the water range that plants can effectively use.

AWC = Field Capacity − Wilting Point

Saturation Point: When all soil pores are filled with water. Beyond this point, water drainage occurs, and root oxygen is limited.
Soil Water Potential (Tension): Indicates how tightly water is held in soil. Measured in kPa, where:

0 kPa = Saturated soil

−33 kPa ≈ Field capacity

−1500 kPa ≈ Wilting point

Soil Texture: Affects water retention. Sandy soils drain quickly, while clay holds more water.

The soil moisture sensor in the transmitter checks the water content in the soil, sends analogue data to the ESP 32, which takes the analogue data and converts it to scientifically accurate values and parameters. These parameters are defined above. There are various ways to program out AI/ML to find out what equation best fits the analogue data, which can be calculated based on heavy experimentation and data collection. The temperature sensor also sends its data to the ESP, and from there, the microcontroller takes the help of the LoRa module to transmit that data to the receiver that is kept far away from the transmitter. The main motto for this transmission is to have data collection and transmission from places in the field where the is no internet connection. One receiver can handle multiple transmitters varying from 4-10(max).

The receiver side is connected to the internet with wifi and can receive the transmitter data, and with the help of an internet connection, sends data to the cloud server that stores data according to the transmitter number, for further analysis. The main role of the receiver is to upload the transmitter's data and also send the data from the cloud back to the respective transmitters in order to control the relay, which controls the valve for irrigation. 

The ML model takes the temperature data and the soil moisture sensor, combined with the local humidity and rainfall fetched from the API. The model predicts when to command the specific valve to turn on, depending on the future rainfall predictions and the current status of the soil moisture. For example, if the soil moisture is below the critical level, then the part of the land should be irrigated immediately. But if the soil moisture is low, but the algorithm predicts that rainfall is expected in a few days, then, depending upon the average evaporation rate and the humidity, only a specific amount of water will be released such that the rainfall replenishes the remaining required amount. This way it saves water.


1. Connect all hardware as per the schematic.
2. Flash "sender.ino" to field ESP32.
3. Flash "receiver.ino" to the base ESP32.
4. Add your Firebase details to "firebase_config.h.h".
5. Power up both ESP32 boards.

Dependencies / Libraries:
- "Firebase_ESP_Client"
- "LoRa by Sandeep Mistry"
- "Adafruit_MCP9808"

Challenges: 
  1. LoRa supports distances up to 15-20km in rural areas, but sometimes fields are larger than that.
  2. Theoretically, one transmitter can cover an area of 250 m^2, but this sample space technique that we assumed here only works for flat land.
  3. Configuring the patch of transmitters if farmers grow different types of crops on different patches of their land is difficult, as one crop can be selected but cannot select multiple crops.
  4. The single transmitter cannot handle so much data at the exact same time and will not upload correct data if we do so.

Future Scope:
  1. The individual transmitters can be coded as nodes of a mesh that can act like nodes that bounce/propel data forward if the distance between any transmitter and the receiver is too long.
  2. The receiver can be coded to generate a sequence of data received, store locally just for the purpose of uploading it to the server(forming a queue structure with the FIFO principle). This way, we can handle a huge amount of load on the receiver and avoid data corruption or system failure.
  3. Solar panels can be installed on each transmitter to make it self-sufficient.
  4. An algorithm can be developed that takes satellite images of the land from the maps API and analyzes the area based on terrain and slope, and finds the best locations to place our transmitters strategically to cover the whole field.

This is a public repo licensed under the MIT license. Users are obliged to reference/credit me(Arsh Singh) when copying the project or using/updating this project.

Made by Arsh Singh

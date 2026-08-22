#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// WiFi credentials
const char* ssid = "Sidd";
const char* password = "12345678";

// MQTT Broker settings
const char* mqtt_server = "broker.emqx.io";  
const int mqtt_port = 1883;                  
const char* mqtt_topic = "emqx/esp8266";     
const char* mqtt_username = "emqx";          
const char* mqtt_password = "public";        

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

// Define pin numbers
#define TRIGPIN D1        // Ultrasonic sensor trigger pin
#define ECHOPIN D2        // Ultrasonic sensor echo pin
#define DIST_LED_PIN D3   // LED for distance threshold

#define DHTPIN D4         // DHT22 data pin
#define TEMP_LED_PIN D5   // LED for temperature threshold

#define PIRPIN D6         // PIR sensor pin
#define PIR_LED_PIN D7    // LED for motion detection

// Constants and thresholds
#define DHTTYPE DHT11     // DHT type
const int distanceThreshold = 30;        // Distance threshold in centimeters
const float temperatureThreshold = 31.5; // Temperature threshold in Celsius

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    dht.begin();

    // Set pin modes
    pinMode(TRIGPIN, OUTPUT);
    pinMode(ECHOPIN, INPUT);
    pinMode(DIST_LED_PIN, OUTPUT);
    
    pinMode(TEMP_LED_PIN, OUTPUT);
    pinMode(PIRPIN, INPUT);
    pinMode(PIR_LED_PIN, OUTPUT);

    connectToWiFi();
    mqtt_client.setServer(mqtt_server, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTTBroker();
}

void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTTBroker();
    }
    mqtt_client.loop();

    collectAndSendSensorData();
    
    delay(2000); // Adjust delay as needed
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the WiFi network");
}

void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void collectAndSendSensorData() {
    // Ultrasonic Sensor Data
    long duration;
    int distance;
    digitalWrite(TRIGPIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGPIN, LOW);
    duration = pulseIn(ECHOPIN, HIGH);
    distance = duration * 0.034 / 2;
    if (distance < distanceThreshold) {
        digitalWrite(DIST_LED_PIN, HIGH); // Turn on LED if distance is below threshold
    } else {
        digitalWrite(DIST_LED_PIN, LOW);
    }

    // DHT22 Sensor Data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    } else {
        if (temperature > temperatureThreshold) {
            digitalWrite(TEMP_LED_PIN, HIGH);
        } else {
            digitalWrite(TEMP_LED_PIN, LOW);
        }
    }

    // PIR Sensor Data
    String motionDetected = "NO";
    if (digitalRead(PIRPIN) == HIGH) {
        digitalWrite(PIR_LED_PIN, HIGH);
        motionDetected = "YES";
    } else {
        digitalWrite(PIR_LED_PIN, LOW);
    }

    String payload = "{";
    payload += "\"temperature\":"; payload += temperature; payload += ",";
    payload += "\"humidity\":"; payload += humidity; payload += ",";
    payload += "\"distance\":"; payload += distance; payload += ",";
    payload += "\"motion\":\""; payload += motionDetected; payload += "\"";
    payload += "}";

    mqtt_client.publish(mqtt_topic, payload.c_str());
    Serial.println("Published sensor data: " + payload);
}
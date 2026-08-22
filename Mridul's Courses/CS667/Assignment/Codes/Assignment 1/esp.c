#include <WiFi.h>        // or #include <ESP8266WiFi.h> for ESP8266
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// MQTT Broker details
const char* mqtt_server = "broker.hivemq.com";  // Replace with your broker's IP or URL
const int mqtt_port = 1883;

// Create a WiFi and MQTT client instance
WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect to the MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ArduinoClient")) {
      Serial.println("connected");
      // Once connected, you can publish or subscribe to topics
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  // Optional: setCallback function to handle messages from subscribed topics
  // client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Example: Publish dummy sensor data
  float distance = 100.0;  // Replace with actual sensor reading
  float temperature = 25.0;
  float humidity = 50.0;
  bool motionDetected = true;

  client.publish("sensors/distance", String(distance).c_str());
  client.publish("sensors/temperature", String(temperature).c_str());
  client.publish("sensors/humidity", String(humidity).c_str());
  client.publish("sensors/motion", motionDetected ? "YES" : "NO");

  delay(2000);  // Publish every 2 seconds
}

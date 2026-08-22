#include "DHT.h"

// Define pin numbers
#define TRIGPIN 4        // Ultrasonic sensor trigger pin
#define ECHOPIN 5        // Ultrasonic sensor echo pin
#define DIST_LED_PIN 8   // LED for distance threshold

#define DHTPIN 2         // DHT22 data pin
#define TEMP_LED_PIN 6   // LED for temperature threshold

#define PIRPIN 3         // PIR sensor pin
#define PIR_LED_PIN 7    // LED for motion detection

// Constants and thresholds
#define DHTTYPE DHT11      // DHT 22 (AM2302)
const int distanceThreshold = 30; // Distance threshold in centimeters
const float temperatureThreshold = 31.5; // Temperature threshold in Celsius

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
  
  // Calibrate PIR sensor
  Serial.print("Calibrating PIR sensor ");
  for (int i = 0; i < 30; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void loop() {
  // Handle Ultrasonic Sensor
  long duration;
  int distance;

  // Trigger the sensor
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  duration = pulseIn(ECHOPIN, HIGH);

  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Distance LED Logic
  if (distance < distanceThreshold) {
    digitalWrite(DIST_LED_PIN, HIGH); // Turn on LED if distance is below threshold
  } else {
    digitalWrite(DIST_LED_PIN, LOW);  // Turn off LED if distance is above threshold
  }

  // Handle DHT22 Sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  
    if (temperature > temperatureThreshold) {
      digitalWrite(TEMP_LED_PIN, HIGH);
    } else {
      digitalWrite(TEMP_LED_PIN, LOW);
    }
  }

  // Handle PIR Sensor
  if (digitalRead(PIRPIN) == HIGH) {
    digitalWrite(PIR_LED_PIN, HIGH);
    Serial.println("Motion detected!");
  } else {
    digitalWrite(PIR_LED_PIN, LOW);
  }

  delay(500); // Short delay to stabilize sensor readings
}
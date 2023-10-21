#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the transceiver module
#define ss 2
#define rst 13
#define dio0 14
#define en 32

// Define the pins for the voltage sensor and HCSR04 sensor
#define voltageSensorPin A0  // Assuming you connect the voltage sensor to A0
#define trigPin 2            // Trigger pin for HCSR04
#define echoPin 5            // Echo pin for HCSR04

int counter = 0;
float voltageValue = 0.0;
long distance = 0;

void setup() {
  // Initiate the LoRa Enable pin
  pinMode(en, OUTPUT);
  // LoRa chip is Active Low
  digitalWrite(en, LOW);

  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  // Setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  // Initialize LoRa communication with a specific frequency (915 MHz for North America).
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }

  // Set the LoRa sync word to 0xF3.
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // Read voltage from the sensor
  int sensorValue = analogRead(voltageSensorPin);
  voltageValue = (sensorValue / 1024.0) * 5.0;  // Assuming 5V reference voltage

  // Read distance from the HCSR04 sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  distance = pulseIn(echoPin, HIGH) / 58;  // Convert to centimeters

  // Display sensor readings in the serial monitor
  Serial.print("Sending packet: ");
  Serial.println(counter);

  Serial.print("Voltage: ");
  Serial.print(voltageValue, 2);  // Print voltage with 2 decimal places
  Serial.print("V, Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  

  // Send LoRa packet to receiver with voltage and distance data
  LoRa.beginPacket();
  LoRa.print("Voltage: ");
  LoRa.print(voltageValue, 2);  // Print voltage with 2 decimal places
  LoRa.print(" V, Distance: ");
  LoRa.print(distance);
  LoRa.endPacket();

  counter++;

  delay(2000);  // Delay for 2 seconds before sending the next packet
}

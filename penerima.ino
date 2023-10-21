#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the pins used by the transceiver module
#define ss 2
#define rst 13
#define dio0 14
#define en 32

// I2C address for the 16x2 LCD
int lcdAddress = 0x27;

// Create an instance of the LiquidCrystal_I2C library
LiquidCrystal_I2C lcd(lcdAddress, 16, 2);

// Define the maximum distance (in centimeters)
const int maxDistance = 30;  // Adjust this value based on your requirement
const int minDistance = 2;

void setup() {
  // Initiate the LoRa Enable pin
  pinMode(en, OUTPUT);
  // LoRa chip is Active Low
  digitalWrite(en, LOW);

  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  // Setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  // Initialize LoRa communication with a specific frequency (915 MHz for North America).
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }

  // Set the LoRa sync word to 0xF3
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  // Initialize the LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Selamat Datang");
  delay(1000);
  lcd.clear();
  lcd.print(" Menerima Data ");
}

void loop() {
  // Try to receive a packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    Serial.println("Received a packet!");

    // Read the received packet into a String
    String receivedData = "";

    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    Serial.println("Received Data: " + receivedData);

    // Extract voltage and distance values from the received data
    int voltageIndex = receivedData.indexOf("Voltage: ");
    int distanceIndex = receivedData.indexOf("Distance: ");
    if (voltageIndex != -1 && distanceIndex != -1) {
      String voltageString = receivedData.substring(voltageIndex + 9, distanceIndex - 2);
      String distanceString = receivedData.substring(distanceIndex + 10);

      // Convert distance to a percentage
      int distance = distanceString.toInt();
      float distancePercentage = map(distance, 0, maxDistance, 100, 0);

      // Convert the distance percentage to a string
      String distancePercentageString = String(distancePercentage, 2);

      // Display voltage and distance percentage on the LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Voltage: " + voltageString );
      lcd.setCursor(0, 1);
      lcd.print("Jagung : " + distancePercentageString + " %");
    }

    // Print an empty line for separation
    Serial.println();
  }
}

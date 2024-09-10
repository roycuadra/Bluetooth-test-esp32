#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

const int ledPin = 2;
bool blinking = false;
unsigned long blinkStartTime = 0;
unsigned long blinkDuration = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT");
  Serial.println("Bluetooth device is ready to pair");
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  if (SerialBT.available()) {
    String receivedText = SerialBT.readStringUntil('\n');
    
    // Clean the received text
    receivedText.trim();
    
    Serial.print("Received: ");
    Serial.println(receivedText);
    
    // Debugging print to check if condition is met
    Serial.print("Command length: ");
    Serial.println(receivedText.length());

    // Print raw bytes to detect hidden characters
    Serial.print("Raw bytes: ");
    for (int i = 0; i < receivedText.length(); i++) {
      Serial.print((byte)receivedText[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    if (receivedText == "ON") {
      digitalWrite(ledPin, HIGH);
      SerialBT.println("LED ON");
      Serial.println("LED turned ON");
    } else if (receivedText == "OFF") {
      digitalWrite(ledPin, LOW);
      SerialBT.println("LED OFF");
      Serial.println("LED turned OFF");
    } else if (receivedText.startsWith("BLINK")) {
      // Extract duration from the command
      int duration = receivedText.substring(6).toInt(); // Extract the number after "BLINK"
      if (duration > 0) {
        blinkDuration = duration * 1000; // Convert to milliseconds
        blinkStartTime = millis(); // Start blinking
        blinking = true;
        SerialBT.println("LED BLINKING");
        Serial.println("LED blinking started");
      } else {
        SerialBT.println("Invalid Duration");
        Serial.println("Invalid duration for BLINK command");
      }
    } else {
      SerialBT.println("Invalid Command");
      Serial.println("Invalid Command received");
    }
  }

  if (Serial.available()) {
    String textToSend = Serial.readStringUntil('\n');
    SerialBT.println(textToSend);
    Serial.print("Sent: ");
    Serial.println(textToSend);
  }

  // Handle blinking
  if (blinking) {
    unsigned long currentTime = millis();
    if (currentTime - blinkStartTime >= blinkDuration) {
      digitalWrite(ledPin, LOW); // Ensure LED is off after blinking
      blinking = false;
      SerialBT.println("LED OFF");
      Serial.println("LED blinking stopped");
    } else {
      // Toggle LED every 500 milliseconds
      if ((currentTime / 500) % 2 == 0) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
    }
  }
}

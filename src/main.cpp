
#include <Arduino.h>  // Arduino framework functions

#define LED_PIN 2      // Onboard LED connected to GPIO2

// Runs once when ESP32 starts or resets
void setup() {
    pinMode(LED_PIN, OUTPUT);  // Configure GPIO2 as output
}

// Runs continuously while ESP32 is powered ON
void loop() {
    digitalWrite(LED_PIN, HIGH);  // Set GPIO2 HIGH (3.3V) - LED ON
    delay(1000);                  // Keep LED ON for 1000 ms (1 second)

    digitalWrite(LED_PIN, LOW);   // Set GPIO2 LOW (0V) - LED OFF
    delay(1000);                  // Keep LED OFF for 1000 ms (1 second)
}

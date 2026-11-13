
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "webpage.h"

// ==========================================
// CONFIGURATION
// ==========================================

#define LED_PIN 2

const char* WIFI_SSID = "podx";
const char* WIFI_PASSWORD = "buck3ye$";

WebServer server(80);
bool ledState = false;


// ==========================================
// LED CONTROL
// ==========================================

void setLED(bool state) {
    ledState = state;

    digitalWrite(LED_PIN, state ? HIGH : LOW);

    Serial.print("LED: ");
    Serial.println(state ? "ON" : "OFF");
}


// ==========================================
// HTTP ROUTES
// ==========================================

void setupRoutes() {

    // Serve web interface
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", WEBPAGE);
    });

    // LED ON
    server.on("/on", HTTP_POST, []() {
        setLED(true);
        server.send(200, "application/json", "{\"led\":true}");
    });

    // LED OFF
    server.on("/off", HTTP_POST, []() {
        setLED(false);
        server.send(200, "application/json", "{\"led\":false}");
    });

    // LED status
    server.on("/status", HTTP_GET, []() {
        String json = "{\"led\":";
        json += ledState ? "true" : "false";
        json += "}";

        server.send(200, "application/json", json);
    });

    // Unknown routes
    server.onNotFound([]() {
        server.send(404, "text/plain", "Not Found");
    });
}


// ==========================================
// SETUP
// ==========================================

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    setLED(false);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    setupRoutes();
    server.begin();

    Serial.println("HTTP server started");
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop() {
    server.handleClient();
    delay(2);
}

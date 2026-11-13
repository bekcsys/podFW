
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "webpage.h"

// ==========================================
// CONFIGURATION
// ==========================================

#define LED_PIN 2

const char* WIFI_SSID = "podx";
const char* WIFI_PASSWORD = "buck3ye$";

// US Central. Change this if the board is in another zone.
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char* TIMEZONE = "CST6CDT,M3.2.0,M11.1.0";
const char* NTP_SERVER_1 = "pool.ntp.org";
const char* NTP_SERVER_2 = "time.nist.gov";

WebServer server(80);
bool ledState = false;
bool clockAnnounced = false;


// ==========================================
// LED CONTROL
// ==========================================

void setLED(bool state) {
    ledState = state;

    digitalWrite(LED_PIN, state ? HIGH : LOW);

    Serial.print("LED: ");
    Serial.println(state ? "ON" : "OFF");
}

String jsonEscape(const String& value) {
    String out;
    out.reserve(value.length() + 4);

    for (unsigned int i = 0; i < value.length(); i++) {
        char c = value.charAt(i);
        if (c == '"' || c == '\\') {
            out += '\\';
        }
        out += c;
    }

    return out;
}

bool clockReady() {
    return time(nullptr) > 1700000000;
}

int timezoneOffsetSeconds(const struct tm& local) {
    char zone[8] = {0};
    strftime(zone, sizeof(zone), "%z", &local);

    if (zone[0] != '+' && zone[0] != '-') {
        return 0;
    }

    int sign = zone[0] == '-' ? -1 : 1;
    int hours = (zone[1] - '0') * 10 + (zone[2] - '0');
    int minutes = (zone[3] - '0') * 10 + (zone[4] - '0');
    return sign * (hours * 3600 + minutes * 60);
}

String statusJson() {
    String ssid = WiFi.SSID();
    if (ssid.length() == 0) {
        ssid = WIFI_SSID;
    }

    time_t now = time(nullptr);
    bool synced = clockReady();
    String timeText = "--:--:--";
    String dateText = "Waiting for network time";
    int offset = 0;

    if (synced) {
        struct tm local;
        localtime_r(&now, &local);

        char timeBuf[16];
        char dateBuf[40];
        strftime(timeBuf, sizeof(timeBuf), "%I:%M:%S %p", &local);
        strftime(dateBuf, sizeof(dateBuf), "%A, %b %d, %Y", &local);

        timeText = timeBuf;
        dateText = dateBuf;
        offset = timezoneOffsetSeconds(local);
    }

    String json = "{\"led\":";
    json += ledState ? "true" : "false";
    json += ",\"ssid\":\"";
    json += jsonEscape(ssid);
    json += "\",\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\",\"rssi\":";
    json += WiFi.RSSI();
    json += ",\"synced\":";
    json += synced ? "true" : "false";
    json += ",\"epoch\":";
    json += static_cast<unsigned long>(now);
    json += ",\"offset\":";
    json += offset;
    json += ",\"time\":\"";
    json += timeText;
    json += "\",\"date\":\"";
    json += dateText;
    json += "\"}";

    return json;
}

void startClock() {
    setenv("TZ", TIMEZONE, 1);
    tzset();
    configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2);
}

void announceClock() {
    if (clockAnnounced || !clockReady()) {
        return;
    }

    clockAnnounced = true;

    struct tm local;
    time_t now = time(nullptr);
    localtime_r(&now, &local);

    Serial.print("Clock set: ");
    Serial.println(&local, "%A, %b %d %Y %I:%M:%S %p");
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

    // LED, Wi-Fi, and device clock
    server.on("/status", HTTP_GET, []() {
        server.send(200, "application/json", statusJson());
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
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    startClock();
    Serial.println("Syncing clock from NTP...");

    setupRoutes();
    server.begin();

    Serial.println("HTTP server started");
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop() {
    server.handleClient();
    announceClock();
    delay(2);
}

/**
 * This sketch connects an AirGradient DIY sensor to a WiFi network, and runs a
 * tiny HTTP server to serve air quality metrics to Prometheus.
 */

#include "main.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <Wire.h>
#include "SSD1306Wire.h"
#include "Configuration/user.h"
#include "Metrics/MetricsGatherer.h"
#include "Prometheus/Server.h"
#include "Configuration/sensors.h"
#include "AQI/Calculator.h"

using Metrics::Gatherer;

// Config ----------------------------------------------------------------------

// For housekeeping.
uint8_t counter = 0;

// Config End ------------------------------------------------------------------

SSD1306Wire display(0x3c, SDA, SCL);
auto metrics = std::make_shared<Gatherer>();
auto aqiCalculator = std::make_shared<AQI::Calculator>(metrics);
auto server = Prometheus::Server(port, metrics, std::shared_ptr<AQI::Calculator>());
Ticker updateScreenTicker;

void setup() {
    Serial.begin(9600);

    // Init Display.
    display.init();
    display.flipScreenVertically();
    showTextRectangle("Init", String(EspClass::getChipId(), HEX), true);

    // Set static IP address if configured.
#ifdef staticip
    WiFi.config(static_ip, gateway, subnet);
#endif

    // Set WiFi mode to client (without this it may try to act as an AP).
    WiFi.mode(WIFI_STA);

    // Configure Hostname
    if ((deviceId != NULL) && (deviceId[0] == '\0')) {
        Serial.printf("No Device ID is Defined, Defaulting to board defaults");
    } else {
        wifi_station_set_hostname(deviceId);
        WiFi.setHostname(deviceId);
    }

    // Setup and wait for WiFi.
    WiFi.begin(ssid, password);
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        showTextRectangle("Trying to", "connect...", true);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Hostname: ");
    Serial.println(WiFi.hostname());
    metrics->setup();

    server.setup();

    showTextRectangle("Listening To", WiFi.localIP().toString() + ":" + String(port), true);
    updateScreenTicker.attach_ms_scheduled(screenUpdateFrequencyMs, updateScreen);
}

void loop() {
    server.loop();
}

// DISPLAY
void showTextRectangle(const String &ln1, const String &ln2, boolean small) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    if (small) {
        display.setFont(ArialMT_Plain_16);
    } else {
        display.setFont(ArialMT_Plain_24);
    }
    display.drawString(32, 16, ln1);
    display.drawString(32, 36, ln2);
    display.display();
}

void updateScreen() {
    auto data = metrics->getData();
    // Take a measurement at a fixed interval.
    switch (counter) {
#ifdef HAS_PM
        case 0:
            showTextRectangle("PM2", String(data.PMS_data.PM_AE_UG_2_5), false);
            break;
#endif
#ifdef HAS_CO2
        case 1:
            showTextRectangle("CO2", String(data.CO2), false);
            break;
#endif
#ifdef HAS_SHT
        case 2:
            showTextRectangle("TMP", String(data.TMP, 1) + "C", false);
            break;
        case 3:
            showTextRectangle("HUM", String(data.HUM, 1) + "%", false);
            break;
#endif
#ifdef HAS_PM
        case 4:
            auto aqi = aqiCalculator->isAQIAvailable() ? String(aqiCalculator->getAQI(), 1) : "N/A";
            showTextRectangle("AQI", aqi, false);
            break;
#endif
    }
    counter = ++counter % 5;
}
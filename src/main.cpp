/**
 * This sketch connects an AirGradient DIY sensor to a WiFi network, and runs a
 * tiny HTTP server to serve air quality metrics to Prometheus.
 */

#include "main.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <Wire.h>
#include "U8g2lib.h"
#include "Configuration/user.h"
#include "Metrics/MetricGatherer.h"
#include "Sensors/Particle/PMSXSensor.h"
#include "Sensors/Temperature/SHTXSensor.h"
#include "Sensors/CO2/SensairS8Sensor.h"
#include "Sensors/Time/BootTimeSensor.h"
#include "AQI/AQICalculator.h"
#include "Prometheus/PrometheusServer.h"
#include "Sensors/TVOC/SGP30Sensor.h"

using namespace AirGradient_Internal;

// Config ----------------------------------------------------------------------


// Config End ------------------------------------------------------------------

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

auto metrics = std::make_shared<MetricGatherer>(-2);
auto aqiCalculator = std::make_shared<AQICalculator>(metrics);
auto server = std::make_unique<PrometheusServer>(port, deviceId, metrics, aqiCalculator);
Ticker updateScreenTicker;


void setup() {
    Serial.begin(9600);

    metrics->addSensor(std::make_unique<PMSXSensor>())
            .addSensor(std::make_unique<SHTXSensor>())
            .addSensor(std::make_unique<SensairS8Sensor>())
            .addSensor(std::make_unique<BootTimeSensor>(ntp_server))
            .addSensor(std::make_unique<SGP30Sensor>(), Measurement::CO2);

    Serial.println("Sensors initiated");
    // Init Display.
    u8g2.begin();

    Serial.println("Screen initiated");


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
        updateOLED2("Trying to", "connect to", ssid);
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

    metrics->begin();
    aqiCalculator->begin();

    server->begin();

    updateOLED2("Listening To", WiFi.localIP().toString() + ":" + String(port), "");
    updateScreenTicker.attach_ms_scheduled(screenUpdateFrequencyMs, updateScreen);
}

void loop() {
    server->handleRequests();
}

void updateOLED2(String ln1, String ln2, String ln3) {
    u8g2.firstPage();
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_t0_16_tf);
        u8g2.drawStr(1, 10, String(ln1).c_str());
        u8g2.drawStr(1, 30, String(ln2).c_str());
        u8g2.drawStr(1, 50, String(ln3).c_str());
    } while (u8g2.nextPage());
}


void updateScreen() {
    auto data = metrics->getData();
    auto sensorType = metrics->getMeasurements();
    auto aqi = aqiCalculator->isAQIAvailable() ? String(aqiCalculator->getAQI(), 1) : "N/A";

    String ln1 = "PM:" + String(data.PARTICLE_DATA.PM_2_5) + " CO2:" + String(data.GAS_DATA.CO2);
    String ln2 = "AQI:" + String(aqi) + " TVOC:" + String(data.GAS_DATA.TVOC);
    String ln3 = "C:" + String(data.TMP) + " H:" + String(data.HUM) + "%";

    updateOLED2(ln1, ln2, ln3);

}
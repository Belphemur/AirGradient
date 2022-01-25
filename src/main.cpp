/**
 * This sketch connects an AirGradient DIY sensor to a WiFi network, and runs a
 * tiny HTTP server to serve air quality metrics to Prometheus.
 */

#include "main.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <Wire.h>
#include "SSD1306Wire.h"
#include <arduino-timer.h>
#include "Configuration/user.h"
#include "Metrics/MetricsGatherer.h"
#include "Prometheus/Server.h"
#include "Configuration/sensors.h"

using Metrics::Gatherer;

// Config ----------------------------------------------------------------------

// For housekeeping.
int counter = 0;

// Config End ------------------------------------------------------------------

SSD1306Wire display(0x3c, SDA, SCL);
auto timer = std::make_shared<Timer<>>();
auto metrics = std::shared_ptr<Gatherer>(&Gatherer::getInstance());
auto server = Prometheus::Server(port, metrics);

void setup()
{
    Serial.begin(9600);

    // Init Display.
    display.init();
    display.flipScreenVertically();
    showTextRectangle("Init", String(ESP.getChipId(), HEX), true);

    metrics->setup(timer);

    // Set static IP address if configured.
#ifdef staticip
    WiFi.config(static_ip, gateway, subnet);
#endif

    // Set WiFi mode to client (without this it may try to act as an AP).
    WiFi.mode(WIFI_STA);

    // Configure Hostname
    if ((deviceId != NULL) && (deviceId[0] == '\0'))
    {
        Serial.printf("No Device ID is Defined, Defaulting to board defaults");
    }
    else
    {
        wifi_station_set_hostname(deviceId);
        WiFi.setHostname(deviceId);
    }

    // Setup and wait for WiFi.
    WiFi.begin(ssid, password);
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED)
    {
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

    server.setup();

    showTextRectangle("Listening To", WiFi.localIP().toString() + ":" + String(port), true);
    timer->every(screenUpdateFrequencyMs, updateScreen);
}

void loop()
{
    timer->tick();
    server.loop();
}

// DISPLAY
void showTextRectangle(String ln1, String ln2, boolean small)
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    if (small)
    {
        display.setFont(ArialMT_Plain_16);
    }
    else
    {
        display.setFont(ArialMT_Plain_24);
    }
    display.drawString(32, 16, ln1);
    display.drawString(32, 36, ln2);
    display.display();
}

bool updateScreen(void *)
{
    auto data = metrics->getData();
    // Take a measurement at a fixed interval.
    switch (counter)
    {
#ifdef HAS_PM
    case 0:
        showTextRectangle("PM2", String(data.PM2), false);
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
        showTextRectangle("HUM", String(data.HUM) + "%", false);
        break;
#endif
    }
    counter++;
    if (counter > 3)
        counter = 0;

    return true;
}
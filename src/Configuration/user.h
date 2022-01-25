#pragma once
#include <ESP8266WiFi.h>

//screen refresh
extern const int screenUpdateFrequencyMs;

//Id of the device for Prometheus
extern const char * deviceId;

//Wifi information
extern const char* ssid;
extern const char* password;
extern const int port;

// Uncomment the line below to configure a static IP address.
//#define staticip

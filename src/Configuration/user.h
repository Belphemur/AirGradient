#pragma once
#include <ESP8266WiFi.h>


//Id of the device for Prometheus
const char * deviceId = "";

//Wifi information
const char* ssid = "";
const char* password = "";
const int port = 9925;

// Uncomment the line below to configure a static IP address.
//#define staticip
#ifdef staticip
IPAddress static_ip(192, 168, 42, 20);
IPAddress gateway(192, 168, 42, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

//Board setup
const bool hasPM = true;
const bool hasCO2 = true;
const bool hasSHT = true;
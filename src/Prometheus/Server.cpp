#include "Server.h"

#include <utility>
#include "Configuration/sensors.h"
#include "Configuration/user.h"

Prometheus::Server::Server(const int serverPort, std::shared_ptr<Metrics::Gatherer> metrics) {
    _serverPort = serverPort;
    _server = std::make_unique<ESP8266WebServer>(_serverPort);
    _metrics = std::move(metrics);
}

Prometheus::Server::~Server() {
    _server->stop();
    _server->close();
}

void Prometheus::Server::loop() {
    _server->handleClient();
}

void Prometheus::Server::setup() {
    _server->on("/", [this] { _handleRoot(); });
    _server->on("/metrics", [this] { _handleRoot(); });
    _server->onNotFound([this] { _handleNotFound(); });

    _server->begin();
    Serial.println("HTTP server started at ip " + WiFi.localIP().toString() + ":" + String(_serverPort));
}

String Prometheus::Server::_generateMetrics() {
    String message = "";
    String idString = _getIdString();

    auto metrics = _metrics->getData();

#ifdef HAS_PM
    message += "# HELP particle_count Count of Particulate Matter in µg/m3\n";
    message += "# TYPE particle_count gauge\n";
    message += "particle_count";
    message += _getIdString("type", "PM2.5");
    message += String(metrics.PMS_data.PM_AE_UG_2_5);
    message += "\n";

    message += "# HELP particle_count Count of Particulate Matter in µg/m3\n";
    message += "# TYPE particle_count gauge\n";
    message += "particle_count";
    message += _getIdString("type", "PM1.0");
    message += String(metrics.PMS_data.PM_AE_UG_1_0);
    message += "\n";

    message += "# HELP particle_count Count of Particulate Matter in µg/m3\n";
    message += "# TYPE particle_count gauge\n";
    message += "particle_count";
    message += _getIdString("type", "PM10.0");
    message += String(metrics.PMS_data.PM_AE_UG_10_0);
    message += "\n";
#endif

#ifdef HAS_CO2
    message += "# HELP rco2 CO2 value, in ppm\n";
    message += "# TYPE rco2 gauge\n";
    message += "rco2";
    message += idString;
    message += String(metrics.CO2);
    message += "\n";

#endif

#ifdef HAS_SHT
    message += "# HELP atmp Temperature, in degrees Celsius\n";
    message += "# TYPE atmp gauge\n";
    message += "atmp";
    message += idString;
    message += String(metrics.TMP);
    message += "\n";

    message += "# HELP rhum Relative humidity, in percent\n";
    message += "# TYPE rhum gauge\n";
    message += "rhum";
    message += idString;
    message += String(metrics.HUM);
    message += "\n";

#endif

#ifdef HAS_BOOT_TIME
    message += "# HELP sensors_boot_time AirGradient boot time, in unixtime.\n";
    message += "# TYPE sensors_boot_time gauge\n";
    message += "sensors_boot_time" + idString + String(metrics.BOOT) + "\n";
#endif

    return message;
}

String Prometheus::Server::_getIdString(const char *labelType, const char *labelValue) const {
    if (labelType == nullptr || labelValue == nullptr)
        return "{id=\"" + String(deviceId) + "\",mac=\"" + WiFi.macAddress().c_str() + "\"}";

    return "{id=\"" + String(deviceId) + "\",mac=\"" + WiFi.macAddress().c_str() + "\"," + labelType + "=\"" +labelValue+ "\"}";
}

void Prometheus::Server::_handleRoot() {
    _server->send(200, "text/plain", _generateMetrics());
}

void Prometheus::Server::_handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server->uri();
    message += "\nMethod: ";
    message += (_server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _server->args();
    message += "\n";
    for (int i = 0; i < _server->args(); i++) {
        message += " " + _server->argName(i) + ": " + _server->arg(i) + "\n";
    }
    _server->send(404, "text/html", message);
}

#pragma once

#include <ESP8266WebServer.h>
#include "Metrics/MetricsGatherer.h"
#include "AQI/Calculator.h"

namespace Prometheus {
    class Server {
    private:
        std::unique_ptr<ESP8266WebServer> _server;
        int _serverPort;
        std::shared_ptr<Metrics::Gatherer> _metrics;
        std::shared_ptr<AQI::Calculator> _aqiCalculator;


        void _handleRoot();

        void _handleNotFound();

        String _generateMetrics();

        String _getIdString(const char *labelType = nullptr, const char *labelValue = nullptr) const;

    public:
        Server(uint16_t serverPort, std::shared_ptr<Metrics::Gatherer> metrics,
               std::shared_ptr<AQI::Calculator> aqiCalculator);

        void loop();

        void setup();

        virtual ~Server();

    };

}

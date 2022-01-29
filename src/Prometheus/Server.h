#pragma once
#include <ESP8266WebServer.h>
#include "Metrics/MetricsGatherer.h"

namespace Prometheus
{
    class Server
    {
    private:
        std::unique_ptr<ESP8266WebServer> _server;
        int _serverPort;
        std::shared_ptr<Metrics::Gatherer> _metrics;

        void _handleRoot();
        void _handleNotFound();
        String _generateMetrics();
        String _getIdString(const char *labelType = nullptr, const char *labelValue = nullptr) const;

    public:
        Server(const int serverPort, std::shared_ptr<Metrics::Gatherer> metrics);
        void loop();
        void setup();
        virtual ~Server();

    };

}

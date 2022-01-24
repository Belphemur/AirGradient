#pragma once
#include <ESP8266WebServer.h>
#include "Metrics/MetricsGatherer.h"

namespace Prometheus
{
    class Server
    {
    private:
        ESP8266WebServer *_server;
        int _serverPort;
        Metrics::Gatherer* _metrics;

        void _handleRoot();
        void _handleNotFound();
        String _generateMetrics();

    public:
        Server(const int serverPort, Metrics::Gatherer* metrics);
        void loop();
        void setup();
        virtual ~Server();
    };

}

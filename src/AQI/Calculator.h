#pragma once

#include "Arduino.h"
#include "Metrics/MetricsGatherer.h"
#include "AQI/MovingAverage.h"

#define GATHER_METRIC_EVERY_X_SECS 900
#define NUM_OF_METRICS_IN_24H 86400/GATHER_METRIC_EVERY_X_SECS


namespace AQI {
    struct Breakpoints {
        uint16_t iHi;
        uint16_t iLo;
        float cHi;
        float cLo;
    };

    class Calculator {
    public:
        Calculator(std::shared_ptr<Metrics::Gatherer> metrics);

        /**
         * Check if we have enough reading to give a proper AQI.
         * (since we need a 24h avg of PM2.5 to do AQI calculation)
         * @return
         */
        bool isAQIAvailable() const { return _average.hasReachCapacity(); }

        /**
         * Get the current AQI, doesn't check if we have enough readings.
         * @return
         */
        float getAQI() const;

        virtual ~Calculator();

    private:
        std::shared_ptr<Metrics::Gatherer> _metrics;
        Ticker _ticker;
        MovingAverage<uint16_t, NUM_OF_METRICS_IN_24H> _average;

        Breakpoints _getPM25Breakpoints(float pm25Avg) const;

        float _getAQI(Breakpoints breakpoints, float pm25Avg) const;

        void _recordMetric();
    };
}




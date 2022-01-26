#pragma once

#include <Ticker.h>
#include <AirGradient.h>

namespace Metrics {
    struct Data {
        int CO2;
        int PM2;
        float TMP;
        int HUM;
    };

    class Gatherer {

    private:
        Ticker _pm2WakerUpTicker;
        Ticker _pm2ReadSleepTicker;
        Ticker _allSensorTicker;
        std::unique_ptr<AirGradient> _airGradient;
        Data _data;

        void _wakeUpPm2();

        void _getPm2DataSleep();

        void _getAllSensorData();

    public:
        Gatherer();

        void setup();

        inline Data getData() const { return _data; }

        virtual ~Gatherer() {
            _data.CO2 = 0;
            _data.HUM = 0;
            _data.PM2 = 0;
            _data.TMP = 0;
        }
    };

}
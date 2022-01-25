#pragma once
#include <arduino-timer.h>
#include <AirGradient.h>

namespace Metrics
{
    struct Data
    {
        int CO2;
        int PM2;
        float TMP;
        int HUM;
    };

    class Gatherer
    {

    private:
        std::shared_ptr<Timer<>> _timer;
        std::unique_ptr<AirGradient> _air_gradient;
        Data _data;
        Gatherer();

    public:
        static Gatherer &getInstance()
        {
            static Gatherer instance; // Guaranteed to be destroyed.
                                      // Instantiated on first use.
            return instance;
        }
        Gatherer(Gatherer const &) = delete;
        void operator=(Gatherer const &) = delete;

        bool _wakeUpPm2(void *);
        bool _getPm2DataSleep(void *);
        bool _getAllSensorData(void *);
        void setup(std::shared_ptr<Timer<>> timer);
        inline const Data getData() { return _data; }
        virtual ~Gatherer() {
            _data.CO2 = 0;
            _data.HUM = 0;
            _data.PM2 = 0;
            _data.TMP = 0;
        }
    };

}
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
        Timer<> *_timer;
        AirGradient *_air_gradient;
        Data _data;
        Gatherer() {}

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
        void setup(Timer<> *timer, AirGradient *air_gradient);
        inline Data getData() { return _data; }
    };

}
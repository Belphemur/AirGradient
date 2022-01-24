#pragma once
#include <arduino-timer.h>
#include <AirGradient.h>

namespace Metrics
{
    struct Data
    {
        int CO2;
        int PM2;
        int TMP;
        int HUM;
    };

    class Gatherer
    {

    private:
        Timer<> *_timer;
        AirGradient *_air_gradient;
        Data _data;

    public:
        Gatherer(Timer<> *timer, AirGradient *air_gradient);
        static Gatherer *instance;
        bool _wakeUpPm2(void *);
        bool _getPm2DataSleep(void *);
        bool _getAllSensorData(void* );
        void loop();
        void setup();
        inline Data getData() { return _data; }
    };

}
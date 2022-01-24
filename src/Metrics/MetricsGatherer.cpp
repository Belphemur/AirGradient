#include "MetricsGatherer.h"
#include "../Configuration/sensors.h"

bool static_wakeUpPm2(void *param)
{
    return Metrics::Gatherer::instance->_wakeUpPm2(param);
}

bool static_getPm2DataSleep(void *param)
{
    return Metrics::Gatherer::instance->_getPm2DataSleep(param);
}

bool static_getAllSensorData(void *param)
{
    return Metrics::Gatherer::instance->_getAllSensorData(param);
}

Metrics::Gatherer::Gatherer(Timer<> *timer, AirGradient *air_gradient)
{
    _timer = timer;
    _air_gradient = air_gradient;
}

void Metrics::Gatherer::loop()
{
    _timer->tick();
}

bool Metrics::Gatherer::_wakeUpPm2(void *param)
{
    _air_gradient->wakeUp();
    _timer->in(PM_SENSOR_DELAY_BEFORE_READING, static_getPm2DataSleep);
    return true;
}

bool Metrics::Gatherer::_getPm2DataSleep(void *param)
{
    _data.PM2 = _air_gradient->getPM2_Raw();
    if (_data.PM2 != 0)
    {
        _air_gradient->sleep();
    }
    return true;
}

bool Metrics::Gatherer::_getAllSensorData(void *param)
{
#ifdef HAS_CO2
    int co2 = _air_gradient->getCO2_Raw();

    while (co2 <= 0 || co2 >= 60000)
    {
        Serial.println("Wrong CO2 reading: " + String(co2));
        co2 = _air_gradient->getCO2_Raw();
        delay(10);
    }

    _data.CO2 = co2;
#endif

#ifdef HAS_SHT

    TMP_RH temp_data = _air_gradient->periodicFetchData();
    _data.TMP = temp_data.t;
    _data.HUM = temp_data.rh;

#endif
    return true;
}

void Metrics::Gatherer::setup()
{
    Metrics::Gatherer::instance = this;
    using namespace std::placeholders; // for `_1`
#ifdef HAS_PM
    _air_gradient->PMS_Init();
    _wakeUpPm2(NULL);
    auto wakup = [&](void *)
    {
        return true;
    };
    _timer->every(PM_SENSOR_PERIOD_MS, static_wakeUpPm2);
#endif
#ifdef HAS_CO2
    _air_gradient->CO2_Init();
#endif
#ifdef HAS_SHT
    _air_gradient->TMP_RH_Init(0x44);
#endif
#ifdef HAS_CO2 || HAS_SHT
    _timer->every(SENSOR_PERIOD_MS, static_getAllSensorData);
#endif
}
#pragma once

#include <Ticker.h>
#include "Configuration/user.h"
#include "NTP/NTPClient.h"
#include "s8_uart.h"
#include "PMS.h"
#include "SHTSensor.h"

#define PMS_BAUDRATE 9600

namespace Metrics {
    struct Data {
        uint16_t CO2 = 0;
        PMS::DATA PMS_data{};
        float TMP = 0;
        float HUM = 0;
#ifdef HAS_BOOT_TIME
        time_t BOOT = 0;
#endif
    };

    class Gatherer {

    private:
        Ticker _pm2WakerUpTicker;
        Ticker _pm2ReadSleepTicker;
        Ticker _allSensorTicker;
        //S8
        std::unique_ptr<SoftwareSerial> _s8SoftwareSerial;
        std::unique_ptr<S8_UART> _s8Sensor;
        //PMS
        std::unique_ptr<SoftwareSerial> _pmsSoftwareSerial;
        std::unique_ptr<PMS> _pms_sensor;
        //SHT
        std::unique_ptr<SHTSensor> _shtSensor;

        Data _data;
        std::unique_ptr<NTP::NTPClient> _ntpClient;

        void _wakeUpPm2();

        void _getPm2DataSleep();

        void _getAllSensorData();

        void init_sensair_S8();

        void init_pms();

        void init_sht();


    public:
        Gatherer();

        void setup();

        inline Data getData() const { return _data; }

        virtual ~Gatherer() {
            _data = {};
        }

    };

}
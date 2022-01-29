#include "MetricsGatherer.h"
#include "Configuration/sensors.h"
#include "Configuration/user.h"

Metrics::Gatherer::Gatherer() {
    _airGradient = std::make_unique<AirGradient>();
#ifdef HAS_BOOT_TIME
    _ntpClient = std::make_unique<NTP::NTPClient>();
#endif
}

void Metrics::Gatherer::_wakeUpPm2() {
    _airGradient->wakeUp();
    _pm2ReadSleepTicker.once_ms_scheduled(PM_SENSOR_DELAY_BEFORE_READING, [this] { _getPm2DataSleep(); });
}

void Metrics::Gatherer::_getPm2DataSleep() {
    auto previousReading = _data.PM2;
    auto reading = _airGradient->getPM2_Raw();

    //Getting -1 mean the sensor isn't ready
    if (reading < 0) {
        return;
    }

    //Sometimes the sensor give 0 when there isn't any value
    //let's check that it's possible because the previous value was already under 10
    if (reading > 0 || previousReading < 10) {
        _data.PM2 = reading;
        _airGradient->sleep();
    }
}

void Metrics::Gatherer::_getAllSensorData() {
#ifdef HAS_CO2
    auto previousReading = _data.CO2;
    auto co2 = _s8_sensor->get_co2();

    while (co2 <= 0 || (previousReading != 0 && abs(co2 - previousReading) >= 500)) {
        Serial.println("Wrong CO2 reading: " + String(co2));
        co2 = _s8_sensor->get_co2();
        delay(10);
    }

    _data.CO2 = co2;
#endif

#ifdef HAS_SHT

    TMP_RH temp_data = _airGradient->periodicFetchData();
    _data.TMP = temp_data.t + SENSOR_TMP_OFFSET;
    _data.HUM = temp_data.rh;

#endif
}

void Metrics::Gatherer::setup() {
#ifdef HAS_BOOT_TIME
    _data.BOOT = _ntpClient->getUtcUnixEpoch();
#endif
#ifdef HAS_PM
    _airGradient->PMS_Init();
    _pm2WakerUpTicker.attach_ms_scheduled(PM_SENSOR_PERIOD_MS, [this] { _wakeUpPm2(); });
#endif
#ifdef HAS_CO2
    init_sensair_S8();
#endif
#ifdef HAS_SHT
    _airGradient->TMP_RH_Init(0x44);
#endif
#if defined(HAS_CO2) || defined(HAS_SHT)
    _allSensorTicker.attach_ms_scheduled(SENSOR_PERIOD_MS, [this] { _getAllSensorData(); });
#endif
#ifdef HAS_PM
    _wakeUpPm2();
#endif
}

void Metrics::Gatherer::init_sensair_S8() {
    _s8_software_serial = std::make_unique<SoftwareSerial>(D4, D3);
    _s8_software_serial->begin(S8_BAUDRATE);
    _s8_sensor = std::make_unique<S8_UART>(*_s8_software_serial);
    // Check if S8 is available
    S8_sensor sensor_data{};
    Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
    Serial.printf("Firmware version: %s\n", sensor_data.firm_version);
    sensor_data.sensor_type_id = _s8_sensor->get_sensor_type_ID();
    Serial.print("Sensor type: 0x");
    printIntToHex(sensor_data.sensor_type_id, 3);
    Serial.println("");
    sensor_data.abc_period = _s8_sensor->get_ABC_period();
    Serial.printf("ABC period: %d\n", sensor_data.abc_period);

}

#include "MetricsGatherer.h"
#include "Configuration/sensors.h"
#include "Configuration/user.h"

Metrics::Gatherer::Gatherer() {
#ifdef HAS_BOOT_TIME
    _ntpClient = std::make_unique<NTP::NTPClient>();
#endif
}

void Metrics::Gatherer::_wakeUpPm2() {
    _pms_sensor->wakeUp();
    _pm2ReadSleepTicker.once_ms_scheduled(PM_SENSOR_DELAY_BEFORE_READING, [this] { _getPm2DataSleep(); });
}

void Metrics::Gatherer::_getPm2DataSleep() {
    auto previousReading = _data.PMS_data.PM_AE_UG_2_5;
    PMS::DATA data{};
    if (!_pms_sensor->readUntil(data, 2000)) {
        Serial.println("Couldn't get a reading of PMS sensor");
        return;
    }

    //Sometimes the sensor give 0 when there isn't any value
    //let's check that it's possible because the previous value was already under 10
    if (data.PM_AE_UG_2_5 > 0 || previousReading < 10) {
        _data.PMS_data = data;
        _pms_sensor->sleep();
    }
}

void Metrics::Gatherer::_getAllSensorData() {
#ifdef HAS_CO2
    auto previousReading = _data.CO2;
    auto co2 = _s8Sensor->get_co2();

    while (co2 <= 0 || (previousReading != 0 && abs(co2 - previousReading) >= 500)) {
        Serial.println("Wrong CO2 reading: " + String(co2));
        co2 = _s8Sensor->get_co2();
        delay(100);
    }

    _data.CO2 = co2;
#endif

#ifdef HAS_SHT

    if (!_shtSensor->readSample()) {
        Serial.println("Can't read SHT sensor data");

    } else {
        _data.TMP = _shtSensor->getTemperature() + SENSOR_TMP_OFFSET;
        _data.HUM = _shtSensor->getHumidity();
    }

#endif
}

void Metrics::Gatherer::setup() {
#ifdef HAS_BOOT_TIME
    _data.BOOT = _ntpClient->getUtcUnixEpoch();
#endif
#ifdef HAS_PM
    init_pms();
#endif
#ifdef HAS_CO2
    init_sensair_S8();
#endif
#ifdef HAS_SHT
    init_sht();
#endif
#if defined(HAS_CO2) || defined(HAS_SHT)
    _allSensorTicker.attach_ms_scheduled(SENSOR_PERIOD_MS, [this] { _getAllSensorData(); });
#endif
}

void Metrics::Gatherer::init_sht() {
    _shtSensor = std::make_unique<SHTSensor>();
    _shtSensor->setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);
    TwoWire wire;
    wire.begin(0x44);
    if(!_shtSensor->init(wire)) {
        Serial.println("Can't init the SHT sensor");
        return;
    }
}

void Metrics::Gatherer::init_pms() {
    _pmsSoftwareSerial = std::make_unique<SoftwareSerial>(D5, D6);
    _pmsSoftwareSerial->begin(PMS_BAUDRATE);
    _pms_sensor = std::make_unique<PMS>(*_pmsSoftwareSerial);

    _wakeUpPm2();

    _pm2WakerUpTicker.attach_ms_scheduled(PM_SENSOR_PERIOD_MS, [this] { _wakeUpPm2(); });
}

void Metrics::Gatherer::init_sensair_S8() {
    _s8SoftwareSerial = std::make_unique<SoftwareSerial>(D4, D3);
    _s8SoftwareSerial->begin(S8_BAUDRATE);
    _s8Sensor = std::make_unique<S8_UART>(*_s8SoftwareSerial);
    // Check if S8 is available
    S8_sensor sensorData{};
    _s8Sensor->get_firmware_version(sensorData.firm_version);
    Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
    Serial.printf("Firmware version: %s\n", sensorData.firm_version);
    sensorData.sensor_type_id = _s8Sensor->get_sensor_type_ID();
    Serial.print("Sensor type: 0x");
    printIntToHex(sensorData.sensor_type_id, 3);
    Serial.println("");
    sensorData.abc_period = _s8Sensor->get_ABC_period();
    Serial.printf("ABC period: %d\n", sensorData.abc_period);

}

#pragma once
#define HAS_PM
#define HAS_CO2
#define HAS_SHT

#ifdef HAS_PM
const unsigned int PM_SENSOR_PERIOD_MS = 120000;
const unsigned int PM_SENSOR_DELAY_BEFORE_READING = 30000;
#endif

#ifdef HAS_CO2 || HAS_SHT
const unsigned int SENSOR_PERIOD_MS = 5000;
#endif
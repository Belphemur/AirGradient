#pragma once
#include <cstdint>
#define HAS_PM
#define HAS_CO2
#define HAS_SHT

#ifdef HAS_PM
extern const unsigned int PM_SENSOR_PERIOD_MS;
extern const unsigned int PM_SENSOR_DELAY_BEFORE_READING;
#endif

#if defined(HAS_CO2) || defined(HAS_SHT)
extern const unsigned int SENSOR_PERIOD_MS;
extern const int8_t SENSOR_TMP_OFFSET;
#endif
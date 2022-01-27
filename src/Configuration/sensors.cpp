#include "sensors.h"

#ifdef HAS_PM
const unsigned int PM_SENSOR_PERIOD_MS = 120000;
const unsigned int PM_SENSOR_DELAY_BEFORE_READING = 30000;
#endif

#if defined(HAS_CO2) || defined(HAS_SHT)
const unsigned int SENSOR_PERIOD_MS = 5000;
const int8_t SENSOR_TMP_OFFSET = -2;
#endif
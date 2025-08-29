#pragma once
#include "sensor.h"

typedef enum {
  HEALTH_STATUS_CRITICAL_LOW,
  HEALTH_STATUS_LOW,
  HEALTH_STATUS_NORMAL,
  HEALTH_STATUS_HIGH,
  HEALTH_STATUS_CRITICAL_HIGH
} healthStatus_t;

typedef struct {
  float thresholds[HEALTH_STATUS_CRITICAL_HIGH + 1];
  sensor_t sensorType;
  sample_t sampleType;
} analyzerConfig_t;

class Analyzer {
  public:
    Analyzer(analyzerConfig_t config);
    inline sensor_t GetSensorType() { return config.sensorType; }
    inline sample_t GetSampleType() { return config.sampleType; }
    healthStatus_t Analyze(Data_t* data);
  private:
    analyzerConfig_t config;
};
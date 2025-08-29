#include "analyzer.h"

Analyzer::Analyzer(analyzerConfig_t config) : config(config) {
}

healthStatus_t Analyzer::Analyze(Data_t* data) {
  healthStatus_t healthStatus = HEALTH_STATUS_NORMAL;

  for (size_t i = 0; i < data->size; i++) {
    for (size_t j = 0; j < HEALTH_STATUS_CRITICAL_HIGH + 1; j++) {
      if (data->data[i] > config.thresholds[j]) {
        healthStatus = (healthStatus_t)j;
        break;
      }
    }
  }

  return healthStatus;
}
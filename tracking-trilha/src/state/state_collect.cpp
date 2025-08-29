#include "state_collect.h"
#include "oximeter.h"
#include "accelerometer.h"


// insert here all wanted_samples
sample_t StateCollect::wanted_samples[] = {
    SAMPLE_TYPE_SPO2,
    SAMPLE_TYPE_HEART_RATE,
    SAMPLE_TYPE_TEMPERATURE,
    SAMPLE_TYPE_ACCEL_X,
    SAMPLE_TYPE_ACCEL_Y,
    SAMPLE_TYPE_ACCEL_Z
};

StateCollect::StateCollect() : State() {
   
}

void StateCollect::Update() {
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (sensorArray[i] != nullptr) {
            sensorArray[i]->Update();
        }
    }

    // Print wanted_samples
    for (size_t sensor_type = 0; sensor_type < SENSOR_TYPE_QTT; sensor_type++) {
        Sensor* sensor = GetSensor((sensor_t)sensor_type);
        if (sensor != nullptr) {
            Data_t data;
            for (size_t sample_index = 0; sample_index < SAMPLE_TYPE_QTT; sample_index++) {
              Analyzer* analyzer = GetAnalyzer((sensor_t)sensor_type, StateCollect::wanted_samples[sample_index]);
              data.type = StateCollect::wanted_samples[sample_index];
              if (sensor->getData(&data)) {
                  printf("Sensor Type: %d, Sample Type: %d, Data: ", sensor_type, data.type);
                  for (size_t buffer_index = 0; buffer_index < data.size; buffer_index++) {
                      printf("%.3f ", data.data[buffer_index]);
                  }
                  if (analyzer != nullptr) {
                      healthStatus_t healthStatus = analyzer->Analyze(&data);
                      printf("Health Status: %d\n", healthStatus);
                  } else {
                    printf("No analyzer found for sensor type: %d\n", sensor_type);
                  }
                  printf("\n");
              }
          }
        }
    }
}

void StateCollect::Pause() {
    // Pause implementation
}

void StateCollect::Resume() {
    // Resume implementation
}

#include "state.h"

State::State() : popRequested(false), quitRequested(false) {
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        sensorArray[i] = nullptr;
        analyzerArray[i] = nullptr;
    }
}

bool State::AddSensor(Sensor* sensor) {
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (sensorArray[i] == nullptr) {
            sensorArray[i] = sensor;
            return true;
        }
    }
    return false;
}

Sensor* State::GetSensor(sensor_t type) {
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (sensorArray[i] != nullptr && sensorArray[i]->GetType() == type) {
            return sensorArray[i];
        }
    }
    return nullptr;
}

bool State::AddAnalyzer(Analyzer* analyzer) {

    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (analyzerArray[i] == nullptr) {
            analyzerArray[i] = analyzer;
            return true;
        }
    }
    return false;
}

Analyzer* State::GetAnalyzer(sensor_t type, sample_t sampleType) {

    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (analyzerArray[i] != nullptr && analyzerArray[i]->GetSensorType() == type && analyzerArray[i]->GetSampleType() == sampleType) {
            return analyzerArray[i];
        }
    }
    return nullptr;
}

State::~State() {
}

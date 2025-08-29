#include "state.h"

State::State() : popRequested(false), quitRequested(false) {
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        sensorArray[i] = nullptr;
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

State::~State() {
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (sensorArray[i] != nullptr) {
            delete sensorArray[i];
            sensorArray[i] = nullptr;
        }
    }
}

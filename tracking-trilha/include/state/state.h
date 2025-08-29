#pragma once

#include <stdint.h>
#include <stdio.h>
#include "sensor.h"
#include "utils.h"

class State {
public:
    State();

    virtual void Update() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;

    virtual ~State();

    virtual bool AddSensor(Sensor* sensor);
    virtual Sensor* GetSensor(sensor_t type);

    inline bool PopRequested() const { return popRequested; }
    inline bool QuitRequested() const { return quitRequested; }
protected:
    Sensor* sensorArray[SENSOR_TYPE_QTT];

    bool popRequested;
    bool quitRequested;
};

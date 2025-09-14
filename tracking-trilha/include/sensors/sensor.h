#pragma once

#include <stdint.h>
#include <stdio.h>

#define USE_GPS

#define MAX_BUFFER_SIZE 10

#define I2C_SPEED_STANDARD	100000
#define I2C_SPEED_FAST		400000

typedef enum sample_t {
    SAMPLE_TYPE_SPO2,
    SAMPLE_TYPE_HEART_RATE,
    SAMPLE_TYPE_TEMPERATURE,
    SAMPLE_TYPE_ACCEL_X,
    SAMPLE_TYPE_ACCEL_Y,
    SAMPLE_TYPE_ACCEL_Z,
#ifdef USE_GPS
    SAMPLE_TYPE_LATITUDE,
    SAMPLE_TYPE_LONGITUDE,
    SAMPLE_TYPE_ALTITUDE,
    SAMPLE_TYPE_SATELLITES,
#endif
    SAMPLE_TYPE_SPEED_KPH,
    SAMPLE_TYPE_QTT
} sample_t;

typedef enum sensor_t {
    SENSOR_TYPE_OXIMETER,
    SENSOR_TYPE_ACCELEROMETER,
#ifdef USE_GPS
    SENSOR_TYPE_GPS,
#endif
    SENSOR_TYPE_QTT
} sensor_t;

typedef struct {
    uint64_t timestamp;
    float *data;
    size_t size;
    sample_t type;
} Data_t;

class Sensor {
  public:
    virtual void Update() = 0;
    virtual bool getData(Data_t* data) = 0;
    virtual inline sensor_t GetType() { return sensorType; }
  protected:
    sensor_t sensorType;
};

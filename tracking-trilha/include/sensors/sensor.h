#pragma once

#include <stdint.h>
#include <stdio.h>

#define USE_GPS
#define USE_SD

#define MAX_BUFFER_SIZE 10

#define I2C_SPEED_STANDARD	100000
#define I2C_SPEED_FAST		400000

typedef enum sample_t {
    SAMPLE_TYPE_SPO2,
    SAMPLE_TYPE_HEART_RATE,
    SAMPLE_TYPE_TEMPERATURE,
#ifdef USE_GPS
    SAMPLE_TYPE_LATITUDE,
    SAMPLE_TYPE_LONGITUDE,
    SAMPLE_TYPE_ALTITUDE,
    SAMPLE_TYPE_SATELLITES,
    SAMPLE_TYPE_SPEED_KPH,
#endif
    SAMPLE_TYPE_ACCEL,
#ifdef USE_GPS
    SAMPLE_TYPE_QTT = 9
#endif
#ifndef USE_GPS
    SAMPLE_TYPE_QTT = 4
#endif
} sample_t;

typedef enum sensor_t {
    SENSOR_TYPE_OXIMETER,
    SENSOR_TYPE_ACCELEROMETER,
#ifdef USE_GPS
    SENSOR_TYPE_GPS,
    SENSOR_TYPE_QTT = 3
#endif
#ifndef USE_GPS
    SENSOR_TYPE_QTT = 2
#endif
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

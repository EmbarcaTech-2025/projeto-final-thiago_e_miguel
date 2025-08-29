#pragma once

#include <stdint.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 128

#define PIN_WIRE_SDA 2
#define PIN_WIRE_SCL 3
#define I2C_PORT i2c1

#define I2C_SPEED_STANDARD	100000
#define I2C_SPEED_FAST		400000

typedef enum sample_t {
    SAMPLE_TYPE_SPO2,
    SAMPLE_TYPE_HEART_RATE,
    SAMPLE_TYPE_TEMPERATURE,
    SAMPLE_TYPE_ACCEL_X,
    SAMPLE_TYPE_ACCEL_Y,
    SAMPLE_TYPE_ACCEL_Z,
    SAMPLE_TYPE_QTT
} sample_t;


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
};

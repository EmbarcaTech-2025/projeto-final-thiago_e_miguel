#pragma once 

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MAX3010X.h"
#include "sensor.h"
#include "algorithm_by_RF.h"

#define PIN_WIRE_SDA_OXI 0
#define PIN_WIRE_SCL_OXI 1
#define I2C_PORT_OXI i2c0

#define MAX3010X_ADDRESS	0x57
#define BUFFER_SIZE_ALGORITHM 100  // only for algorithm_by_RF.h

class Oximeter : public Sensor {
  public:
    Oximeter();

    void Update();
    bool getData(Data_t* data);
  private:
    bool is_valid();

    float buffer_spO2[MAX_BUFFER_SIZE];  //SPO2 value
    float buffer_heart_rate[MAX_BUFFER_SIZE];  //Heart rate value
    float buffer_temperature[MAX_BUFFER_SIZE];  //Temperature value
    size_t buffer_size_spO2 = 0;
    size_t buffer_size_heart_rate = 0;
    size_t buffer_size_temperature = 0;

    int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
    int32_t n_heart_rate; //heart rate value
    int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
    float n_spo2;
};

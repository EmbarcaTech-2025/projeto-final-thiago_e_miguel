#pragma once 

#include <stdio.h>
#include "pico/stdlib.h"
#include "GPS_GY-NEO6MV2.h"
#include "sensor.h"


class GPS : public Sensor {
  public:
    GPS();

    void Update();
    bool getData(Data_t* data);
  private:
    bool setDataWithBuffer(Data_t* data, float* buffer, size_t* buffer_size);

    float buffer_lat[MAX_BUFFER_SIZE];
    float buffer_lon[MAX_BUFFER_SIZE];
    float buffer_alt[MAX_BUFFER_SIZE];
    float buffer_sat[MAX_BUFFER_SIZE];
    float buffer_spd[MAX_BUFFER_SIZE];
    
    size_t buffer_size_lat = 0;
    size_t buffer_size_lon = 0;
    size_t buffer_size_alt = 0;
    size_t buffer_size_sat = 0;
    size_t buffer_size_spd = 0;

    GPS_GYNEO6MV2 gpsModule;
};

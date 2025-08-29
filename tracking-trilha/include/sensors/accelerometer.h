#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "sensor.h"
#include "imu6050.h"

class Accelerometer : public Sensor {
  public:
    Accelerometer();

    void Update();
    bool getData(Data_t* data);
  private:
    float buffer_accel_x[MAX_BUFFER_SIZE];  // Accelerometer X value
    float buffer_accel_y[MAX_BUFFER_SIZE];  // Accelerometer Y value
    float buffer_accel_z[MAX_BUFFER_SIZE];  // Accelerometer Z value
    size_t buffer_size_accel_x = 0;
    size_t buffer_size_accel_y = 0;
    size_t buffer_size_accel_z = 0;

    IMU6050 imuSensor = IMU6050(I2C_PORT, PIN_WIRE_SDA, PIN_WIRE_SCL, I2C_SPEED_FAST, MPU_ADDR);
};
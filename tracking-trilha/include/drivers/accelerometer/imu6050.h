#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>


#define MPU_ADDR 0x68

#define MPU_ACCEL_XOUT_H 0x3B
#define MPU_ACCEL_XOUT_L 0x3C
#define MPU_ACCEL_YOUT_H 0x3D
#define MPU_ACCEL_YOUT_L 0x3E
#define MPU_ACCEL_ZOUT_H 0x3F
#define MPU_ACCEL_ZOUT_L 0x40

#define MPU_TEMP_OUT_H 0x41
#define MPU_TEMP_OUT_L 0x42

#define MPU_GYRO_XOUT_H 0x43
#define MPU_GYRO_XOUT_L 0x44
#define MPU_GYRO_YOUT_H 0x45
#define MPU_GYRO_YOUT_L 0x46
#define MPU_GYRO_ZOUT_H 0x47
#define MPU_GYRO_ZOUT_L 0x48

// Definições para conversão de unidades
#define ACCEL_RANGE_2G (0x8000 / 2.0f)  // ±2g
#define ACCEL_RANGE_4G (0x8000 / 4.0f)   // ±4g
#define ACCEL_RANGE_8G (0x8000 / 8.0f)   // ±8g
#define ACCEL_RANGE_16G (0x8000 / 16.0f)  // ±16g

#define GYRO_RANGE_250 (0x8000 / 250.0f)    // ±250°/s
#define GYRO_RANGE_500 (0x8000 / 500.0f)     // ±500°/s
#define GYRO_RANGE_1000 (0x8000 / 1000.0f)    // ±1000°/s
#define GYRO_RANGE_2000 (0x8000 / 2000.0f)    // ±2000°/s

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} imu6050_data_t;

typedef struct {
  float x;
  float y;
  float z;
} imu6050_calibrated_t;

class IMU6050 {
public:
    IMU6050(i2c_inst_t* i2c_type, uint8_t sdata , uint8_t sclk , uint32_t i2cSpeed = 400000, uint8_t i2cAddr = MPU_ADDR);
    bool begin();

    // Função para ler dados do IMU6050
    void read_imu6050(imu6050_data_t *data, const uint8_t *registers, uint8_t num_registers);
    uint16_t read_imu6050_reg(const uint8_t *registers, uint8_t num_registers);

    // Funções auxiliares
    void read_accelerometer(imu6050_data_t *accel_data);
    void read_gyroscope(imu6050_data_t *gyro_data);
    float read_temperature();

    // Funções para converter dados brutos em unidades físicas
    void convert_accelerometer_data(imu6050_data_t *raw_data, imu6050_calibrated_t *calibrated_data);
    void convert_gyroscope_data(imu6050_data_t *raw_data, imu6050_calibrated_t *calibrated_data);   
private:
    i2c_inst_t *_i2c;
    uint8_t  _i2caddr;
    uint8_t _SDataPin;
    uint8_t _SClkPin;
    uint32_t _CLKSpeed;

};
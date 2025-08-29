#include "imu6050.h"

IMU6050::IMU6050(i2c_inst_t* i2c_type, uint8_t sdata , uint8_t sclk , uint32_t i2cSpeed, uint8_t i2cAddr) {
    _i2c = i2c_type;
    _i2caddr = i2cAddr;
    _SDataPin = sdata;
    _SClkPin = sclk;
    _CLKSpeed = i2cSpeed;
}

bool IMU6050::begin() {
    // Inicializar I2C
    i2c_init(_i2c, _CLKSpeed);
    gpio_set_function(_SDataPin, GPIO_FUNC_I2C);
    gpio_set_function(_SClkPin, GPIO_FUNC_I2C);
    gpio_pull_up(_SDataPin);
    gpio_pull_up(_SClkPin);
    
    // Aguardar um pouco para estabilização
    sleep_ms(100);
    
    // Wake up MPU6050 (sair do modo sleep)
    uint8_t wake_cmd[] = {0x6B, 0x00}; // PWR_MGMT_1 register, clear sleep bit
    if (i2c_write_blocking(_i2c, _i2caddr, wake_cmd, 2, false) != 2) {
        return false; // Falha na comunicação I2C
    }
    
    // Configurar range do acelerômetro (±2g)
    uint8_t accel_config[] = {0x1C, 0x00}; // ACCEL_CONFIG register
    if (i2c_write_blocking(_i2c, _i2caddr, accel_config, 2, false) != 2) {
        return false; // Falha na comunicação I2C
    }
    
    // Configurar range do giroscópio (±250°/s)
    uint8_t gyro_config[] = {0x1B, 0x00}; // GYRO_CONFIG register
    if (i2c_write_blocking(_i2c, _i2caddr, gyro_config, 2, false) != 2) {
        return false; // Falha na comunicação I2C
    }
    
    sleep_ms(100);
    return true;
}

void IMU6050::read_imu6050(imu6050_data_t *data, const uint8_t *registers, uint8_t num_registers) {
    uint8_t buffer[6]; // Buffer para 6 bytes (3 eixos x 2 bytes cada)
    
    // Escrever endereço do primeiro registro
    i2c_write_blocking(_i2c, _i2caddr, (uint8_t*)registers, 1, true);
    
    // Ler dados
    i2c_read_blocking(_i2c, _i2caddr, buffer, num_registers, false);
    
    // Converter bytes para valores de 16 bits
    data->x = (buffer[0] << 8) | buffer[1];
    data->y = (buffer[2] << 8) | buffer[3];
    data->z = (buffer[4] << 8) | buffer[5];
}

uint16_t IMU6050::read_imu6050_reg(const uint8_t *registers, uint8_t num_registers) {
    uint8_t data[2];
    
    // Escrever endereço do registro
    i2c_write_blocking(_i2c, _i2caddr, (uint8_t*)registers, 1, true);
    
    // Ler dados
    i2c_read_blocking(_i2c, _i2caddr, data, num_registers, false);
    
    if (num_registers == 2) {
        return (data[0] << 8) | data[1];
    } else {
        return data[0];
    }
}

void IMU6050::read_accelerometer(imu6050_data_t *accel_data) {
    uint8_t accel_reg = MPU_ACCEL_XOUT_H;
    read_imu6050(accel_data, &accel_reg, 6);
}

void IMU6050::read_gyroscope(imu6050_data_t *gyro_data) {
    uint8_t gyro_reg = MPU_GYRO_XOUT_H;
    read_imu6050(gyro_data, &gyro_reg, 6);
}

float IMU6050::read_temperature() {
    uint8_t temp_reg = MPU_TEMP_OUT_H;
    uint16_t raw_temp = read_imu6050_reg(&temp_reg, 2);
    
    // Converter valor bruto para graus Celsius
    return (raw_temp / 340.0f) + 36.53f;
}

void IMU6050::convert_accelerometer_data(imu6050_data_t *raw_data, imu6050_calibrated_t *calibrated_data) {
    calibrated_data->x = raw_data->x / ACCEL_RANGE_2G; // Convertendo para g
    calibrated_data->y = raw_data->y / ACCEL_RANGE_2G;
    calibrated_data->z = raw_data->z / ACCEL_RANGE_2G;
}

void IMU6050::convert_gyroscope_data(imu6050_data_t *raw_data, imu6050_calibrated_t *calibrated_data) {
    calibrated_data->x = raw_data->x / GYRO_RANGE_250; // Convertendo para °/s
    calibrated_data->y = raw_data->y / GYRO_RANGE_250;
    calibrated_data->z = raw_data->z / GYRO_RANGE_250;
}

#include "accelerometer.h"
#include <string.h>
#include "utils.h"

Accelerometer::Accelerometer() : Sensor() {
  busy_wait_ms(500);
    while (imuSensor.begin() != true)
    {
        printf("IMU6050 not connect r fail load calib coeff \r\n");
        busy_wait_ms(500);
    }

    sensorType = SENSOR_TYPE_ACCELEROMETER;
}

void Accelerometer::Update() {
    imu6050_data_t raw_data;
    imu6050_calibrated_t calibrated_data;

    // Ler dados do acelerômetro
    imuSensor.read_accelerometer(&raw_data);

    // Converter dados brutos para unidades físicas (g)
    imuSensor.convert_accelerometer_data(&raw_data, &calibrated_data);

    if (buffer_size_accel_x >= MAX_BUFFER_SIZE) {
      shift_buffer(buffer_accel_x, &buffer_size_accel_x);
    }
    if (buffer_size_accel_y >= MAX_BUFFER_SIZE) {
      shift_buffer(buffer_accel_y, &buffer_size_accel_y);
    }
    if (buffer_size_accel_z >= MAX_BUFFER_SIZE) {
      shift_buffer(buffer_accel_z, &buffer_size_accel_z);
    }
    // Armazenar dados nos buffers
    buffer_accel_x[buffer_size_accel_x++] = calibrated_data.x;
    buffer_accel_y[buffer_size_accel_y++] = calibrated_data.y;
    buffer_accel_z[buffer_size_accel_z++] = calibrated_data.z;
}

bool Accelerometer::getData(Data_t* data) {
    switch (data->type) {
        case SAMPLE_TYPE_ACCEL_X:
            if (buffer_size_accel_x == 0) {
                return false;
            }
            data->data = buffer_accel_x;
            data->size = buffer_size_accel_x;
            buffer_size_accel_x = 0;
            break;
        case SAMPLE_TYPE_ACCEL_Y:
            if (buffer_size_accel_y == 0) {
                return false;
            }
            data->data = buffer_accel_y;
            data->size = buffer_size_accel_y;
            buffer_size_accel_y = 0;
            break;
        case SAMPLE_TYPE_ACCEL_Z:
            if (buffer_size_accel_z == 0) {
                return false;
            }
            data->data = buffer_accel_z;
            data->size = buffer_size_accel_z;
            buffer_size_accel_z = 0;
            break;
        default:
            return false;
    }

    data->timestamp = to_ms_since_boot(get_absolute_time());
    return true;
}
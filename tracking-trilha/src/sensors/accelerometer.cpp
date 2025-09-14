#include "accelerometer.h"
#include <string.h>
#include "utils.h"
#include "hardware/clocks.h"
#include <math.h>
#include "../../include/utils/debug_printf.h"

Accelerometer::Accelerometer() : Sensor() {
  busy_wait_ms(500);
    while (imuSensor.begin() != true)
    {
        PRINTF_DEBUG("IMU6050 not connect r fail load calib coeff \r\n");
        busy_wait_ms(500);
    }

    sensorType = SENSOR_TYPE_ACCELEROMETER;
    buffer_size_accel_x = 0;
    buffer_size_accel_y = 0;
    buffer_size_accel_z = 0;
    buffer_size_accel_magnitude = 0;
    
    // Initialize FreeRTOS components
    dataMutex = xSemaphoreCreateMutex();
}

Accelerometer::~Accelerometer() {
    if (dataMutex != nullptr) {
        vSemaphoreDelete(dataMutex);
    }
}

void Accelerometer::Update() {
    imu6050_data_t raw_data;
    imu6050_calibrated_t calibrated_data;

    //PRINTF_DEBUG("Accelerometer Time: %llu\n", get_absolute_time());

    // Ler dados do acelerômetro
    //PRINTF_DEBUG("Read Accelerometer\n");
    imuSensor.read_accelerometer(&raw_data);

    // Converter dados brutos para unidades físicas
    //PRINTF_DEBUG("Convert Accelerometer Data\n");
    imuSensor.convert_accelerometer_data(&raw_data, &calibrated_data);
    
    // Validate data before processing
    if (isnan(calibrated_data.x) || isnan(calibrated_data.y) || isnan(calibrated_data.z)) {
        PRINTF_DEBUG("ERROR: Invalid accelerometer data detected!\n");
        return;
    }

    // Take mutex to safely update shared data
    if (dataMutex == nullptr) {
        PRINTF_DEBUG("ERROR: dataMutex is null!\n");
        return;
    }
    
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        // Implement circular buffer behavior - always shift when at max size
        if (buffer_size_accel_x >= MAX_BUFFER_SIZE) {
          //PRINTF_DEBUG("Shift Accelerometer X\n");
          shift_buffer(buffer_accel_x, &buffer_size_accel_x);
        }
        if (buffer_size_accel_y >= MAX_BUFFER_SIZE) {
          //PRINTF_DEBUG("Shift Accelerometer Y\n");
          shift_buffer(buffer_accel_y, &buffer_size_accel_y);
        }
        if (buffer_size_accel_z >= MAX_BUFFER_SIZE) {
          //PRINTF_DEBUG("Shift Accelerometer Z\n");
          shift_buffer(buffer_accel_z, &buffer_size_accel_z);
        }
        if (buffer_size_accel_magnitude >= MAX_BUFFER_SIZE) {
          //PRINTF_DEBUG("Shift Accelerometer Magnitude\n");
          shift_buffer(buffer_accel_magnitude, &buffer_size_accel_magnitude);
        }
        // PRINTF_DEBUG("Buffer Size Accelerometer X: %d\n", buffer_size_accel_x);
        // PRINTF_DEBUG("Buffer Size Accelerometer Y: %d\n", buffer_size_accel_y);
        // PRINTF_DEBUG("Buffer Size Accelerometer Z: %d\n", buffer_size_accel_z);
        // PRINTF_DEBUG("Buffer Size Accelerometer Magnitude: %d\n", buffer_size_accel_magnitude);
        // PRINTF_DEBUG("Store Accelerometer Data\n");
        // Armazenar dados nos buffers
        buffer_accel_x[buffer_size_accel_x++] = calibrated_data.x;
        buffer_accel_y[buffer_size_accel_y++] = calibrated_data.y;
        buffer_accel_z[buffer_size_accel_z++] = calibrated_data.z;
        buffer_accel_magnitude[buffer_size_accel_magnitude++] = sqrtf(calibrated_data.x * calibrated_data.x + calibrated_data.y * calibrated_data.y + calibrated_data.z * calibrated_data.z);

        xSemaphoreGive(dataMutex);
    } else {
        PRINTF_DEBUG("ERROR: Failed to acquire dataMutex!\n");
    }

    // PRINTF_DEBUG("End Accelerometer Time: %llu\n", get_absolute_time());
}

bool Accelerometer::getData(Data_t* data) {
    // Take mutex to safely access shared data
    if (dataMutex == nullptr) {
        PRINTF_DEBUG("ERROR: dataMutex is null in getData!\n");
        return false;
    }
    
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        bool result = false;
        
        switch (data->type) {
            case SAMPLE_TYPE_ACCEL:
                if (buffer_size_accel_magnitude == 0) {
                    PRINTF_DEBUG("No accelerometer data available\n");
                    result = false;
                } else {
                    // PRINTF_DEBUG("Consuming %d accelerometer samples\n", buffer_size_accel_magnitude);
                    data->data = buffer_accel_magnitude;
                    data->size = buffer_size_accel_magnitude;
                    // Reset buffer after reading to prevent reprocessing same data
                    buffer_size_accel_magnitude = 0;
                    result = true;
                }
                break;
            default:
                result = false;
        }

        data->timestamp = to_ms_since_boot(get_absolute_time());
        
        xSemaphoreGive(dataMutex);
        return result;
    } else {
        PRINTF_DEBUG("ERROR: Failed to acquire dataMutex in getData!\n");
    }
    
    return false;
}
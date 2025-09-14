#pragma once 

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MAX3010X.h"
#include "sensor.h"
#include "algorithm_by_RF.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define PIN_WIRE_SDA_OXI 2
#define PIN_WIRE_SCL_OXI 3
#define I2C_PORT_OXI i2c1

#define MAX3010X_ADDRESS	0x57
#define BUFFER_SIZE_ALGORITHM 25  // only for algorithm_by_RF.h - reduced for faster processing

// FreeRTOS task configuration
#define OXIMETER_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define OXIMETER_TASK_STACK_SIZE 2048
#define OXIMETER_UPDATE_PERIOD_MS 1000  // Update every 1 second

class Oximeter : public Sensor {
  public:
    Oximeter();
    ~Oximeter();

    void Update();
    bool getData(Data_t* data);
    void StartTask();
    void StopTask();
    
  private:
    bool is_valid();
    static void OximeterTask(void* pvParameters);
    void UpdateInternal();

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
    
    // FreeRTOS task management
    TaskHandle_t taskHandle;
    SemaphoreHandle_t dataMutex;
    bool taskRunning;
};

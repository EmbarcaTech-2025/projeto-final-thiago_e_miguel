#include <stdio.h>
#include "include/utils/debug_printf.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oximeter.h"
#include "accelerometer.h"
#include "state_collect.h"
#include "analyzer.h"
#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "SD.h"


#define TICK_PERIOD_MS 100 // ms

int main(void) {
    stdio_init_all();
    sleep_ms(5000);
    PRINTF_DEBUG("Starting main...\n");

    StateCollect stateCollect;
    Oximeter oximeter = Oximeter();
    Accelerometer accelerometer = Accelerometer();
    SD sd = SD();

    stateCollect.setSD(&sd);


    analyzerConfig_t accelerometerConfig = {
        .thresholds = {0.0f, 0.5f, 0.75f, 1.2f, 1.5f},
        .sensorType = SENSOR_TYPE_ACCELEROMETER,
        .sampleType = SAMPLE_TYPE_ACCEL
    };

    Analyzer accelerometerAnalyzer = Analyzer(accelerometerConfig);

    analyzerConfig_t oximeterConfig = {
        .thresholds = {0.0f, 90.0f, 98.0f, 200.0f, 200.0f},
        .sensorType = SENSOR_TYPE_OXIMETER,
        .sampleType = SAMPLE_TYPE_SPO2
    };
    
    Analyzer oximeterAnalyzer = Analyzer(oximeterConfig);

    analyzerConfig_t heartRateConfig = {
        .thresholds = {0.0f, 60.0f, 100.0f, 140.0f, 180.0f},
        .sensorType = SENSOR_TYPE_OXIMETER,
        .sampleType = SAMPLE_TYPE_HEART_RATE
    };

    Analyzer heartRateAnalyzer = Analyzer(heartRateConfig);

    analyzerConfig_t temperatureConfig = {
        .thresholds = {0.0f, 35.0f, 37.0f, 39.0f, 41.0f},
        .sensorType = SENSOR_TYPE_OXIMETER,
        .sampleType = SAMPLE_TYPE_TEMPERATURE
    };
    
    Analyzer temperatureAnalyzer = Analyzer(temperatureConfig); 

    stateCollect.AddSensor(&oximeter);
    stateCollect.AddSensor(&accelerometer);

    stateCollect.AddAnalyzer(&oximeterAnalyzer);
    stateCollect.AddAnalyzer(&accelerometerAnalyzer);

    stateCollect.AddAnalyzer(&heartRateAnalyzer);
    stateCollect.AddAnalyzer(&temperatureAnalyzer);

    stateCollect.setSamplesFilename("samples.csv");

    Oled oled;

    oled.Clear();

    stateCollect.setOled(&oled);

    // Start the oximeter task
    sleep_ms(1000);
    PRINTF_DEBUG("Starting oximeter task...\n");
    oximeter.StartTask();

    // Start the state collection task
    sleep_ms(500);
    PRINTF_DEBUG("Starting state collection task...\n");
    stateCollect.StartTask();

    // Start the FreeRTOS scheduler
    PRINTF_DEBUG("Starting FreeRTOS scheduler...\n");
    vTaskStartScheduler();

    // This should never be reached
    PRINTF_DEBUG("ERROR: FreeRTOS scheduler stopped unexpectedly!\n");
    return 0;
}
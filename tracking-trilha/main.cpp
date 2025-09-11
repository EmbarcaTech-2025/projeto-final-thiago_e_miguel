#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oximeter.h"
#include "accelerometer.h"
#include "state_collect.h"
#include "analyzer.h"
#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"

#define TICK_PERIOD_MS 100 // ms

int main(void) {
    stdio_init_all();
    sleep_ms(5000);
    printf("Starting main...\n");

    StateCollect stateCollect;
    Oximeter oximeter = Oximeter();
    Accelerometer accelerometer = Accelerometer();


    analyzerConfig_t accelerometerConfig = {
        .thresholds = {0.0f, 0.5f, 0.75f, 1.2f, 1.5f},
        .sensorType = SENSOR_TYPE_ACCELEROMETER,
        .sampleType = SAMPLE_TYPE_ACCEL_X
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

    stateCollect.AddSensor(&oximeter);
    stateCollect.AddSensor(&accelerometer);

    stateCollect.AddAnalyzer(&oximeterAnalyzer);
    stateCollect.AddAnalyzer(&accelerometerAnalyzer);

    stateCollect.AddAnalyzer(&heartRateAnalyzer);

    Oled oled;

    oled.Clear();

    stateCollect.setOled(&oled);

    // Start the oximeter task
    sleep_ms(1000);
    printf("Starting oximeter task...\n");
    oximeter.StartTask();

    // Start the state collection task
    sleep_ms(500);
    printf("Starting state collection task...\n");
    stateCollect.StartTask();

    // Start the FreeRTOS scheduler
    printf("Starting FreeRTOS scheduler...\n");
    vTaskStartScheduler();

    // This should never be reached
    printf("ERROR: FreeRTOS scheduler stopped unexpectedly!\n");
    return 0;
}
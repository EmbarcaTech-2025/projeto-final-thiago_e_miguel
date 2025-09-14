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
#ifdef USE_GPS
#include "gps.h"
#endif
#include "SD.h"
#include <string.h>
#include <stdlib.h>

#define TICK_PERIOD_MS 100 // ms

int main(void) {
    stdio_init_all();
    sleep_ms(5000);
    PRINTF_DEBUG("Starting main...\n");

    StateCollect stateCollect;
    Oximeter oximeter = Oximeter();
    Accelerometer accelerometer = Accelerometer();
#ifdef USE_GPS
    GPS gps = GPS();
#endif
    SD sd = SD();

    stateCollect.setSD(&sd); 

    stateCollect.AddSensor(&oximeter);
    stateCollect.AddSensor(&accelerometer);
#ifdef USE_GPS
    stateCollect.AddSensor(&gps);
#endif

    stateCollect.setSamplesFilename("samples.csv");

    // oled.Clear();

    // stateCollect.setOled(&oled);

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
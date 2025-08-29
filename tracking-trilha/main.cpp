#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oximeter.h"
#include "accelerometer.h"
#include "state_collect.h"
#include "analyzer.h"
#include "oled.h"

#define TICK_PERIOD_MS 1000 // ms

int main(void) {
    stdio_init_all();

    Oled oled;

    oled.Clear();
    oled.PrintText(2, "Iniciando...");
    oled.DrawLine(0, 0, 127, 63, true);
    oled.Render();

    sleep_ms(2000);

    StateCollect stateCollect;

    Oximeter oximeter;
    Accelerometer accelerometer;

    analyzerConfig_t oximeterAnalyzerConfig = {
        .thresholds = {0.0, 0.0, 0.0, 0.0, 0.0},
        .sensorType = SENSOR_TYPE_OXIMETER
    };

    analyzerConfig_t accelerometerAnalyzerConfig = {
        .thresholds = {0.0, 0.0, 0.0, 0.0, 0.0},
        .sensorType = SENSOR_TYPE_ACCELEROMETER
    };

    Analyzer oximeterAnalyzer(oximeterAnalyzerConfig);
    Analyzer accelerometerAnalyzer(accelerometerAnalyzerConfig);

    stateCollect.AddSensor(&oximeter);
    stateCollect.AddSensor(&accelerometer);

    stateCollect.AddAnalyzer(&oximeterAnalyzer);
    stateCollect.AddAnalyzer(&accelerometerAnalyzer);

    while (1) {
        stateCollect.Update();
        sleep_ms(TICK_PERIOD_MS);
    }
    return 0;
}
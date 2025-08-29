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

	while (1) {
        stateCollect.Update();

        sleep_ms(TICK_PERIOD_MS);
	}
	return 0;
}
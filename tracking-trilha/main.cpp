#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oximeter.h"
#include "accelerometer.h"

#define TICK_PERIOD_MS 1000 // ms

typedef enum {
    STATE_CONFIG,
    STATE_COLLECT,
    STATE_ALERT,
} State_t;

int main(void) {

	stdio_init_all();

    State_t state = STATE_COLLECT;
    Oximeter* oximeter = new Oximeter();
    Accelerometer* accelerometer = new Accelerometer();
    Sensor* sensor_array[] = {
        oximeter,
        accelerometer,
    };

	while (1) {
        for (Sensor* sensor : sensor_array) {
            sensor->Update();
        }

        Data_t data;
        data.type = SAMPLE_TYPE_SPO2;
        if (oximeter->getData(&data)) {
            printf("SPO2: ");
            for (size_t i = 0; i < data.size; i++) {
                printf("%.1f %%", data.data[i]);
            }
            printf("\n");
        } else {
            printf("No SPO2 data available.\n");
        }

        data.type = SAMPLE_TYPE_HEART_RATE;
        if (oximeter->getData(&data)) {
            printf("Heart Rate: ");
            for (size_t i = 0; i < data.size; i++) {
                printf("%.1f BPM", data.data[i]);
            }
            printf("\n");
        } else {
            printf("No Heart Rate data available.\n");
        }

        data.type = SAMPLE_TYPE_TEMPERATURE;
        if (oximeter->getData(&data)) {
            printf("Temperature: ");
            for (size_t i = 0; i < data.size; i++) {
                printf("%.1f °C", data.data[i]);
            }
            printf("\n");
        } else {
            printf("No Temperature data available.\n");
        }

        data.type = SAMPLE_TYPE_ACCEL_X;
        if (accelerometer.getData(&data)) {
            printf("Accel X: ");
            for (size_t i = 0; i < data.size; i++) {
                printf("%.3f g", data.data[i]);
            }
            printf("\n");
        } else {
            printf("No Accel X data available.\n");
        }

        data.type = SAMPLE_TYPE_ACCEL_Y;
        if (accelerometer.getData(&data)) {
            printf("Accel Y: ");
            for (size_t i = 0; i < data.size; i++) {
                printf("%.3f g", data.data[i]);
            }
            printf("\n");
        } else {
            printf("No Accel Y data available.\n");
        }

        data.type = SAMPLE_TYPE_ACCEL_Z;
        if (accelerometer.getData(&data)) {
            printf("Accel Z: ");
            for (size_t i = 0; i < data.size; i++) {
                printf("%.3f g", data.data[i]);
            }
            printf("\n");
        } else {
            printf("No Accel Z data available.\n");
        }

        sleep_ms(TICK_PERIOD_MS);
	}
	return 0;
}
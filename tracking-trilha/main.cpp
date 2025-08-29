#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oximeter.h"
#include "accelerometer.h"
#include "state_collect.h"

#define TICK_PERIOD_MS 1000 // ms

int main(void) {

	stdio_init_all();

    StateCollect stateCollect;

	while (1) {
        stateCollect.Update();

        sleep_ms(TICK_PERIOD_MS);
	}
	return 0;
}
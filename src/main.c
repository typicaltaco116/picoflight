#include <stdio.h>
#include <stdint.h>
#include <pico/stdlib.h>

#include <picoflight/actuators/dshotx4.h>

#define DSHOT_PINBASE 6
#define DSHOT_SPEED 300

int main(void) 
{
    stdio_init_all();

    dshotx4_t *motors = dshotx4Construct(DSHOT_PINBASE, 4, DSHOT_SPEED, pio0, 0);

    while (1) {
        sleep_us(200); // must not send commands too quickly.
        dshotx4SendThrottles(motors, (uint16_t[]){0, 0, 0, 0});
    }

    return 0;
}

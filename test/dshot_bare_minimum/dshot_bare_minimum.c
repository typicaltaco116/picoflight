#include <stdint.h>
#include <pico/stdlib.h>

#include <picoflight/actuators/dshotx4.h>

#define DSHOT_PINBASE 2
#define DSHOT_SPEED 300

int main(void) 
{
    stdio_init_all();

    dshotx4Init(DSHOT_PINBASE, 4, DSHOT_SPEED, pio0, 0);

    for (uint32_t i = 0; i < 50000; ++i) {
        sleep_us(200); // must not send commands too quickly.
        dshotx4SendRawThrottles((uint16_t[]){0, 0, 0, 0});
    }

    while (1) {
        sleep_us(200);
        dshotx4SendRawThrottles((uint16_t[]){1, 1, 1, 1});
    }


    return 0;
}

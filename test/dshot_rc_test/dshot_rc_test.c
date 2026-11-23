// Local Headers
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/rc/sbus.h>
#include <picoflight/core/utilities.h>

// pico-sdk Headers
#include <pico/stdlib.h>
#include <stdio.h>

#define SBUS_RX_PIN 9
#define SBUS_UART_INST uart1
#define DSHOT_PINBASE 2
#define DSHOT_SPEED 300

int main(void)
{
    stdio_init_all();

    sleep_ms(1000);
    sbusInitBus(SBUS_UART_INST, SBUS_RX_PIN);
    dshotx4Init(DSHOT_PINBASE, 4, DSHOT_SPEED, pio0, 0);

    while (1) {
        sleep_us(200);

        rc_t ch;
        if (sbusGetChannels(ch))
            printf("%0.2f %0.2f %0.2f %0.2f\r\n", ch[1], ch[2], ch[3], ch[4]);

        float throttleVal = mapf(ch[3], -0.75, 0.85, -1.0, 1.0);

        // Throttle is not zero at zero!!!
        if (sbusGetIsFailsafe() || ch[3] < -0.75) {
            dshotx4SendRawThrottles((uint16_t[]){0, 0, 0, 0});
            continue;
        }

        dshotx4SendNormThrottles((float[]){
            throttleVal, throttleVal, throttleVal, throttleVal});
    }
}

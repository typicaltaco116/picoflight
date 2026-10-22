// Local Headers
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/RC/SBUS.h>

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
    SBUS_InitBus(SBUS_UART_INST, SBUS_RX_PIN);
    dshotx4_t *motors = dshotx4Construct(DSHOT_PINBASE, 4, DSHOT_SPEED, pio0, 0);

    while (1) {
        sleep_us(200);

        RC_t ch;
        if (SBUS_GetChannels(ch))
            printf("%0.2f %0.2f %0.2f %0.2f\r\n", ch[1], ch[2], ch[3], ch[4]);

        // Throttle is not zero at zero!!!
        if (SBUS_IsFailsafe() || ch[3] < 0.2) {
            dshotx4SendRawThrottles(motors, (uint16_t[]){0, 0, 0, 0});
            continue;
        }

        uint16_t throttleVal = (float)(ch[3]) * 2000.0;
        dshotx4SendThrottles(motors, (uint16_t[]){
            throttleVal, throttleVal, throttleVal, throttleVal});
    }
}

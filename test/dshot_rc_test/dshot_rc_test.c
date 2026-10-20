// Local Headers
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/RC/SBUS.h>

// pico-sdk Headers
#include <pico/stdlib.h>

#define SBUS_RX_PIN 9
#define SBUS_UART_INST uart1
#define DSHOT_PINBASE 2
#define DSHOT_SPEED 300

int main(void)
{
    SBUS_InitBus(SBUS_UART_INST, SBUS_RX_PIN);
    dshotx4_t *motors = dshotx4Construct(DSHOT_PINBASE, 4, DSHOT_SPEED, pio0, 0);

    while (1) {
        sleep_us(200);

        RC_t ch;
        SBUS_GetChannels(ch);
        if (SBUS_IsFailsafe()) {
            dshotx4SendRawThrottles(motors, (uint16_t[]){0, 0, 0, 0});
            continue;
        }

        uint16_t throttleVal = (float)(ch[3]) * 2000.0;
        dshotx4SendThrottles(motors, (uint16_t[]){
            throttleVal, throttleVal, throttleVal, throttleVal});
    }
}

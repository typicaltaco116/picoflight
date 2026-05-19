#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "src/RC/RC.h"
#include "src/RC/SBUS.h"
#include "src/actuators/servo.h"

#define SBUS_RX 9
#define SBUS_UART_INST uart1
#define CHANNEL_3_SERVO_OUTPUT_PIN 2

int main() 
{
    stdio_init_all();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    SBUS_InitBus(SBUS_UART_INST, SBUS_RX);

    servo_t out3 = servo_Register(CHANNEL_3_SERVO_OUTPUT_PIN);
    servo_Init(out3);
    servo_Drive(out3, 0);

    while (1) {
        RC_t ch;
        if (SBUS_GetChannels(ch)) {
            printf("%0.2f %0.2f %0.2f %0.2f\r\n", ch[1], ch[2], ch[3], ch[4]);
            servo_Drive(out3, ch[3]);
        }
        if (SBUS_IsFailsafe()) {
            puts("Failsafe");
        }
        sleep_ms(1);

    }
}

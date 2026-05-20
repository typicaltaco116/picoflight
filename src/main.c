#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "src/actuators/servo.h"

#define CHANNEL_3_SERVO_OUTPUT_PIN 2

int main() 
{
    stdio_init_all();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    servo_t out3 = servo_Register(CHANNEL_3_SERVO_OUTPUT_PIN);
    servo_Init(out3);
    servo_Drive(out3, 0);

    while (1) {
        servo_Drive(out3, -1.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
        servo_Drive(out3, 0.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
        servo_Drive(out3, 1.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
    }
}

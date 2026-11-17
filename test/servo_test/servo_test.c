// Local Headers.
#include <picoflight/actuators/pwm.h>

// PICO-sdk Headers.
#include <pico/stdlib.h>

// Standard Headers.
#include <stdio.h>
#include <stdint.h>

#define CHANNEL_3_SERVO_OUTPUT_PIN 2

int main() 
{
    stdio_init_all();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    pwm_t out3 = pwmRegister(CHANNEL_3_SERVO_OUTPUT_PIN);
    pwmInit(out3);
    pwmDrive(out3, 0);

    while (1) {
        pwmDrive(out3, -1.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
        pwmDrive(out3, 0.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
        pwmDrive(out3, 1.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
    }
}

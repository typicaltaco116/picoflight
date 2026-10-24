// Local Headers.
#include <picoflight/actuators/PWM.h>

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

    PWM_t out3 = PWMRegister(CHANNEL_3_SERVO_OUTPUT_PIN);
    PWMInit(out3);
    PWMDrive(out3, 0);

    while (1) {
        PWMDrive(out3, -1.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
        PWMDrive(out3, 0.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
        PWMDrive(out3, 1.0f);
        while (stdio_getchar_timeout_us(100) != (int)'\r');
    }
}

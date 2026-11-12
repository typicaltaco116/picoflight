// Local Headers
#include <picoflight/core/cli.h>
#include <picoflight/core/configHelpers.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdio.h>

int main(void) 
{
    stdio_init_all();

    while (!stdio_usb_connected())
        sleep_ms(100);

    PID_gains_t defaultValue = (PID_gains_t){
        .p = 1.0f,
        .i = 2.0f,
        .d = 3.0f,
        .all = 4.0f
    };
    configGetIfPIDGains("TEST", defaultValue);

    cliProcessInput();

    printf("Done ...\n");
    while (1);
}

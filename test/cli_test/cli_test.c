// Local Headers
#include <picoflight/core/cli.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdio.h>

int main(void) 
{
    stdio_init_all();

    while (!stdio_usb_connected())
        sleep_ms(100);

    cliProcessInput();

    printf("Done ...\n");
    while (1);
}

// Local Headers
#include <picoflight/config/flash.h>

// PICO-sdk Headers
#include <pico/stdlib.h>

// Standard Headers;
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

extern char __config_reserved_flash;

int main(void) 
{
    stdio_init_all();

    configRegisterParam("Version", sizeof(int));
    configRegisterParam("PID I Gain", sizeof(float));

    while (1) {
        char c = '\0';
        while (c != '1' && c != '2')
            c = (char)stdio_getchar_timeout_us(100);

        if (c == '1') {
            puts("Writing parameters to flash.");

            if (configSetParam("Version", (int[]){16}) != EXIT_SUCCESS)
                puts("Error: SetParam failed");
            if (configSetParam("PID I Gain", (float[]){1.25}) != EXIT_SUCCESS)
                puts("Error: SetParam failed");

            int *version = (int*)configGetParam("Version");
            float *kI = (float*)configGetParam("PID I Gain");

            if (!version)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("Version = %d (RAM)\r\n", *version);
            if (!kI)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("PID I Gain = %f (RAM)\r\n", *kI);

            configWriteAll();

        } else {
            puts("Retrieving parameters from flash.");

            configReadAll();

            int *version = (int*)configGetParam("Version");
            float *kI = (float*)configGetParam("PID I Gain");

            if (!version)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("Version = %d (Flash)\r\n", *version);
            if (!kI)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("PID I Gain = %f (Flash)\r\n", *kI);
        }

        printf("Done...\r\n");
    }
}

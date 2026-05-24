#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "src/config/params.h"
#include <string.h>

extern char __config_reserved_flash;

int main(void) 
{
    stdio_init_all();

    config_RegisterParam("Version", sizeof(int));
    config_RegisterParam("PID I Gain", sizeof(float));

    while (1) {
        char c = '\0';
        while (c != '1' && c != '2')
            c = (char)stdio_getchar_timeout_us(100);

        if (c == '1') {
            puts("Writing parameters to flash.");

            if (config_SetParam("Version", (int[]){16}) != EXIT_SUCCESS)
                puts("Error: SetParam failed");
            if (config_SetParam("PID I Gain", (float[]){1.25}) != EXIT_SUCCESS)
                puts("Error: SetParam failed");

            int *version = (int*)config_GetParam("Version");
            float *kI = (float*)config_GetParam("PID I Gain");

            if (!version)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("Version = %d (RAM)\r\n", *version);
            if (!kI)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("PID I Gain = %f (RAM)\r\n", *kI);

            config_WriteAll();

        } else {
            puts("Retrieving parameters from flash.");

            config_ReadAll();

            int *version = (int*)config_GetParam("Version");
            float *kI = (float*)config_GetParam("PID I Gain");

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

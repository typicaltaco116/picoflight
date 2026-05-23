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

    while (1) {
        char c = '\0';
        while (c != '1' && c != '2')
            c = (char)stdio_getchar_timeout_us(100);

        printf("&__config_reserved_flash = %p\r\n", &__config_reserved_flash);

        if (c == '1') {
            puts("Writing parameters to flash.");

            if (config_SetParam("Version", (int[]){16}) != EXIT_SUCCESS)
                puts("Error: SetParam failed");

            int *version = (int*)config_GetParam("Version");

            if (!version)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("Version = %d (RAM)\r\n", *version);

            config_WriteAll();

        } else {
            puts("Retrieving parameters from flash.");

            config_ReadAll();

            int *version = (int*)config_GetParam("Version");

            if (!version)
                puts("Error: Failed to retrieve parameter from config table");
            else
                printf("Version = %d (RAM)\r\n", *version);
        }

        printf("Done...\r\n");
    }
}

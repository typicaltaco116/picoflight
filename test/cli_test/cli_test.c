// Local Headers
#include <picoflight/core/cli.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdio.h>
#include <stdint.h>

void blockingReadlineStripped(char *buffer, uint32_t bufferSize)
{
    uint32_t idx = 0;

    while (idx < bufferSize - 1) {
        int c = getchar();
        if (c == PICO_ERROR_TIMEOUT || c == '\n') { // makes sure \r is read.
            break;
        }
        else {
            buffer[idx++] = (char)c;
        }
    }

    buffer[idx - 1] = '\0';
}

int main(void) 
{
    stdio_init_all();

    while (1) {
        char buffer[128];
        printf("PicoFlightCLI: ");
        blockingReadlineStripped(buffer, sizeof(buffer));

        cliExecuteString(buffer);
    }
    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "src/actuators/dshot.h"

#ifndef DSHOT_PIN
#define DSHOT_PIN 5
#endif

int main(void) 
{
    dshot_controller_t motor1;

    stdio_init_all();
    
    dshot_ControllerInit(&motor1, 300, pio0, 0, DSHOT_PIN, 1);
    dshot_Throttle(&motor1, 0, 0);

    while (1) {
        while (getc(stdin) != '\r');

        dshot_Command(&motor1, 0, DSHOT_CMD_BEEP1); 
    }

    return 0;
}

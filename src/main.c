#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "RC/RC.h"
#include "RC/SBUS.h"

#define SBUS_RX 9
#define SBUS_UART_INST uart1

int main() 
{
    stdio_init_all();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    SBUS_InitBus(SBUS_UART_INST, SBUS_RX);

    while (1) {
        RC_t ch;
        if (SBUS_GetChannels(ch)) {
            printf("%0.2f %0.2f %0.2f %0.2f\r\n", ch[1], ch[2], ch[3], ch[4]);
        }
        if (SBUS_IsFailsafe()) {
            puts("Failsafe");
        }
        sleep_ms(1);

    }
}

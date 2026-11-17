// Local Headers
#include <picoflight/RC/rc.h>
#include <picoflight/RC/sbus.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdio.h>
#include <stdint.h>

#define SBUS_RX_PIN 9
#define SBUS_UART_INST uart1

int main() 
{
    stdio_init_all();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    sbusInitBus(SBUS_UART_INST, SBUS_RX_PIN);

    while (1) {
     rc_t ch;
        if (sbusGetChannels(ch))
            printf("%0.2f %0.2f %0.2f %0.2f\r\n", ch[1], ch[2], ch[3], ch[4]);
        if (sbusGetIsFailsafe())
            puts("Failsafe");
        sleep_ms(1);
    }
}

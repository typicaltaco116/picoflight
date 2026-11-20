// Local Headers
#include <picoflight/rc/rc.h>
#include <picoflight/rc/sbus.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SBUS_RX_PIN 9
#define SBUS_UART_INST uart1

int main() 
{
    stdio_init_all();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    sbusInitBus(SBUS_UART_INST, SBUS_RX_PIN);

    rc_t pCh;
    while (1) {
        sleep_ms(10);

        rc_t ch;
        if (sbusGetChannels(ch)) {
            memcpy(pCh, ch, 18 * sizeof(float));
        }

        if (sbusGetIsFailsafe()) {
            printf("Failsafe\n");
            continue;
        }

        printf("%0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f \n",
                pCh[1],
                pCh[2],
                pCh[3],
                pCh[4],
                pCh[5],
                pCh[6],
                pCh[7],
                pCh[8],
                pCh[9],
                pCh[10],
                pCh[11],
                pCh[12],
                pCh[13],
                pCh[14],
                pCh[15],
                pCh[16]
        );
    }
}

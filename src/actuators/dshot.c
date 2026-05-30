#include "dshot.h"

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pico_pio_loader/pico_pio_loader.h"

#include "dshot.pio.h"

#include <stdbool.h>

#define DSHOT_MIN_THROTTLE_COMMAND (48)
#define DSHOT_MAX_THROTTLE_COMMAND (2047)

static int pio_sm = -1;
static PIO pio;
static uint pio_offset;

bool dshot_Init(uint dshot_gpio, bool enable_repeat)
{
    pio_sm = pio_claim_unused_sm(pio, false);

    if (pio_sm < 0) {
        return false;
    }

    if (!pio_loader_add_or_get_offset(pio, &dshot_program, &pio_offset)) {
        pio_sm_unclaim(pio, pio_sm);
        pio_sm = -1;
        return false;
    }

    dshot_program_init(pio, pio_sm, pio_offset, dshot_gpio, enable_repeat);
    return true;
}

void dshot_SendCommand(uint16_t c)
{
    // Shift for telemetry bit (0)
    c = c << 1;

    // Shift and include checksum
    uint16_t checksum = (c ^ (c >> 4) ^ (c >> 8)) & 0x0F;
    c = (c << 4) | checksum;

    pio_sm_put_blocking(pio, pio_sm, c);
}

void dshot_SendThrottle(float t)
{
    dshot_SendCommand(dshot_GetThrottleCommand(t));
}

uint16_t dshot_GetThrottleCommand(float t)
{
    uint16_t command;

    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;

    command = DSHOT_MIN_THROTTLE_COMMAND + 
        t * (DSHOT_MAX_THROTTLE_COMMAND - DSHOT_MIN_THROTTLE_COMMAND);

    if (command < DSHOT_MIN_THROTTLE_COMMAND)
        command = DSHOT_MIN_THROTTLE_COMMAND;
    if (command > DSHOT_MAX_THROTTLE_COMMAND)
        command = DSHOT_MAX_THROTTLE_COMMAND;

    return command;
}

void dshot_Stop()
{
    // Signal PIO to wait for the next push
    pio_sm_put_blocking(pio, pio_sm, 0);
}

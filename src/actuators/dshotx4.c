#include <picoflight/actuators/dshotx4.h>

// PICO-sdk Headers
#include <hardware/pio.h>
#include <hardware/clocks.h>

// Standard Headers
#include <stdint.h>
#include <stdlib.h>

// PIO assembly generated Header.
#include "dshotx4.pio.h"


dshotx4_t* dshotx4Construct(
        uint8_t pinBase,
        uint8_t pinCount,
        uint32_t speed,
        PIO pio,
        uint8_t sm)
{
    if (speed != 150 && speed != 300 && speed != 600 && speed != 1200)
        return NULL;

    if (pio_sm_is_claimed(pio, sm))
        return NULL;

    if (!pio_can_add_program(pio, &dshotx4_program))
        return NULL;

    dshotx4_t *obj = calloc(1, sizeof(dshotx4_t));

    obj->pio = pio;
    obj->pinBase = pinBase;
    obj->pinCount = pinCount;
    obj->sm = sm;
    obj->speed = speed;

    obj->offset = pio_add_program(pio, &dshotx4_program);

    for (uint8_t i = 0; i < pinCount; ++i) {
        uint8_t pin = pinBase + i;
        pio_gpio_init(pio, pin);
        gpio_set_pulls(pin, false, false); // subject to change.
    }

    pio_sm_config cfg = dshotx4_program_get_default_config(obj->offset);
    sm_config_set_set_pins(&cfg, pinBase, pinCount);
	sm_config_set_out_pins(&cfg, pinBase, pinCount);
	sm_config_set_out_shift(&cfg, false, false, 32);
	pio_sm_set_consecutive_pindirs(pio, obj->sm, pinBase, pinCount, true);
	pio_sm_init(pio, obj->sm, obj->offset, &cfg);
	pio_sm_set_enabled(pio, obj->sm, true);

    uint32_t targetClock = 12000000 / 300 * speed;
    uint32_t cpuClock = clock_get_hz(clk_sys);
    pio_sm_set_clkdiv(pio, obj->sm, (float)cpuClock / targetClock);

    return obj;
}


void dshotx4Free(dshotx4_t *obj)
{
    pio_sm_set_enabled(obj->pio, obj->sm, false);

    pio_sm_unclaim(obj->pio, obj->sm);

    pio_remove_program(obj->pio, &dshotx4_program, obj->offset);

    // Free any gpio pins
    for (uint8_t i = 0; i < obj->pinCount; ++i)
        gpio_init(obj->pinBase + i);

    free(obj);
}


static uint16_t appendTelemetry(uint16_t data)
{
    return (data << 1) | 0; // Appends a high telemetry bit;
}


static uint16_t appendChecksum(uint16_t data)
{
	uint16_t checksum = data;
	checksum ^= data >> 4;
	checksum ^= data >> 8;
	checksum &= 0x000F;
	return (data << 4) | checksum;
}


// Consider rework with the fact that this code needs obj->pinCount must be 4.
void dshotx4SendRawThrottles(dshotx4_t *obj, uint16_t *data)
{
    for (uint8_t i = 0; i < 4; ++i) {
        data[i] = appendTelemetry(data[i]);
        data[i] = appendChecksum(data[i]);
    }

    uint32_t packet[2] = {0};
    for (int32_t i = 31; i >= 0; --i) {
        int32_t pos = i / 4;
        int32_t motor = i % 4;
        packet[0] |= ((data[motor] >> (pos + 8)) & 1) << i;
        packet[1] |= ((data[motor] >> pos) & 1) << i;
    }

    pio_sm_put(obj->pio, obj->sm, packet[0]);
    pio_sm_put(obj->pio, obj->sm, packet[1]);
}


void dshotx4SendThrottles(dshotx4_t *obj, uint16_t *data)
{
    for (uint8_t i = 0; i < obj->pinCount; ++i) {
        if (data[i] > 2000)
            data[i] = 2000;
        if (data[i] != 0) // Keep zero throttle zero.
            data[i] += 47;
    }

    dshotx4SendRawThrottles(obj, data);
}


void dshotx4SendCommand(dshotx4_t *obj, dshotCommands_e cmd)
{
    dshotx4SendRawThrottles(obj, (uint16_t[]){cmd, cmd, cmd, cmd});
}

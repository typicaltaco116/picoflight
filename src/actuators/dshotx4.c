// Primary Header
#include <picoflight/actuators/dshotx4.h>

// Local Headers
#include <picoflight/core/utilities.h>

// PICO-sdk Headers
#include <hardware/pio.h>
#include <hardware/clocks.h>

// Standard Headers
#include <stdint.h>

// PIO assembly generated Header.
#include "dshotx4.pio.h"

static dshotx4_t _motors;

void dshotx4Init(
        uint8_t pinBase,
        uint8_t pinCount,
        uint32_t speed,
        PIO pio,
        uint8_t sm)
{
    if (speed != 150 && speed != 300 && speed != 600 && speed != 1200)
        while (1);

    if (pio_sm_is_claimed(pio, sm))
        while (1);

    if (!pio_can_add_program(pio, &dshotx4_program))
        while (1);

    _motors.pio = pio;
    _motors.pinBase = pinBase;
    _motors.pinCount = pinCount;
    _motors.sm = sm;
    _motors.speed = speed;

    _motors.offset = pio_add_program(pio, &dshotx4_program);

    for (uint8_t i = 0; i < pinCount; ++i) {
        uint8_t pin = pinBase + i;
        pio_gpio_init(pio, pin);
        gpio_set_pulls(pin, false, false); // subject to change.
    }

    pio_sm_config cfg = dshotx4_program_get_default_config(_motors.offset);
    sm_config_set_set_pins(&cfg, pinBase, pinCount);
	sm_config_set_out_pins(&cfg, pinBase, pinCount);
	sm_config_set_out_shift(&cfg, false, false, 32);
	pio_sm_set_consecutive_pindirs(pio, _motors.sm, pinBase, pinCount, true);
	pio_sm_init(pio, _motors.sm, _motors.offset, &cfg);
	pio_sm_set_enabled(pio, _motors.sm, true);

    uint32_t targetClock = 12000000 / 300 * speed;
    uint32_t cpuClock = clock_get_hz(clk_sys);
    pio_sm_set_clkdiv(pio, _motors.sm, (float)cpuClock / targetClock);
}

void dshotx4Destroy(void)
{
    pio_sm_set_enabled(_motors.pio, _motors.sm, false);

    pio_sm_unclaim(_motors.pio, _motors.sm);

    pio_remove_program(_motors.pio, &dshotx4_program, _motors.offset);

    // Free any gpio pins
    for (uint8_t i = 0; i < _motors.pinCount; ++i)
        gpio_init(_motors.pinBase + i);
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

void dshotx4SendRawThrottles(uint16_t *data)
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

    pio_sm_put(_motors.pio, _motors.sm, packet[0]);
    pio_sm_put(_motors.pio, _motors.sm, packet[1]);
}


void dshotx4SendThrottles(uint16_t *data)
{
    for (uint8_t i = 0; i < _motors.pinCount; ++i) {
        if (data[i] > 2000)
            data[i] = 2000;
        if (data[i] != 0) // Keep zero throttle zero.
            data[i] += 47;
    }

    dshotx4SendRawThrottles(data);
}

void dshotx4SendNormThrottles(float *data)
{
    uint16_t intThrottles[4];
    for (uint8_t i = 0; i < _motors.pinCount; ++i) {
        intThrottles[i] = mapf(data[i], -1.0f, 1.0f, 0, 2000);
        if (intThrottles[i] > 2000)
            intThrottles[i] = 2000;
        if (intThrottles[i] != 0)
            intThrottles[i] += 47;
    }
    dshotx4SendRawThrottles(intThrottles);
}


void dshotx4SendCommand(dshotCommands_e cmd)
{
    dshotx4SendRawThrottles((uint16_t[]){cmd, cmd, cmd, cmd});
}

void dshotx4SendCommands(dshotCommands_e *cmd)
{
    dshotx4SendRawThrottles((uint16_t[]){cmd[0], cmd[1], cmd[2], cmd[3]});
}

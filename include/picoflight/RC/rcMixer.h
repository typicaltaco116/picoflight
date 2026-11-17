#pragma once

// Local Headers
#include <picoflight/RC/rc.h>
#include <picoflight/orientation/types.h>

// pico-sdk Headers
#include <pico/types.h>

// Standard Headers
#include <stdbool.h>

typedef struct {
    bool enabled;
    float trimNorm;
    float inMin;
    float inMax;
} rcMixerChannelParameters_t;

void rcMixerInit(void);

void rcMixerSetChannelParameters(uint ch, rcMixerChannelParameters_t params);

void rcMixerRunInPlace(rc_t channels);

void rcMixerSetRates(
    float roll, // units of degrees per second.
    float pitch,
    float yaw
);
void rcMixerSetAngleLimits(float roll, float pitch); // units of degrees.;

void rcMixerSetAxisMapping(uint rollCh, uint pitchCh, uint yawCh);

bodyRates_t rcMixerGetRateSetpoints(rc_t inputRC);
bodyAngles_t rcMixerGetAngleSetpoints(rc_t inputRC);

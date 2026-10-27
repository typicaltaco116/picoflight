#pragma once

// Local Headers
#include <picoflight/RC/RC.h>
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
} RC_mixer_channel_parameters_t;

void RCMixerInit(void);

void RCMixerSetChannelParameters(uint ch, RC_mixer_channel_parameters_t params);

void RCMixerRunInPlace(RC_t channels);

void RCMixerSetRates(
    float roll, // units of degrees per second.
    float pitch,
    float yaw
);
void RCMixerSetAngleLimits(float roll, float pitch); // units of degrees.;

void RCMixerSetAxisMapping(uint rollCh, uint pitchCh, uint yawCh);

rates_t RCMixerGetRateSetpoints(RC_t inputRC);
euler_t RCMixerGetAngleSetpoints(RC_t inputRC);

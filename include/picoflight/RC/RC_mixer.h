#pragma once

// Local Headers
#include <picoflight/RC/RC.h>

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

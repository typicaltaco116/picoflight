// Primary Header
#include <picoflight/RC/RC_mixer.h>

// Local Headers
#include <picoflight/RC/RC.h>
#include <picoflight/core/utilities.h>

// pico-sdk Headers
#include <pico/types.h>

// Standard Headers
#include <stdbool.h>

static RC_mixer_channel_parameters_t _channel_params[RC_MAX_CH]; // Zero-indexed

void RCMixerInit(void)
{
    for (uint i = 0; i < RC_MAX_CH; ++i) {
        _channel_params[i].enabled = false;
    }
}

void RCMixerSetChannelParameters(uint ch, RC_mixer_channel_parameters_t params)
{
    if (ch + 1 > RC_MAX_CH)
        return;

    _channel_params[ch + 1] = params;
}

void RCMixerRunInPlace(RC_t channels)
{
    for (uint i = 0; i < RC_MAX_CH; ++i) {
        if (!_channel_params[i].enabled)
            continue;

        channels[i + 1] = mapf(channels[i + 1], 
            _channel_params[i].inMin, _channel_params[i].inMax, 
            -1.0f, 1.0f
        );

        channels[i + 1] += _channel_params[i].trimNorm;

        channels[i + 1] = constrainf(channels[i + 1], -1.0f, 1.0f);
    }
}

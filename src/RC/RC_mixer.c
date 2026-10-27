// Primary Header
#include <picoflight/RC/RC_mixer.h>

// Local Headers
#include <picoflight/RC/RC.h>
#include <picoflight/core/utilities.h>
#include <picoflight/orientation/types.h>

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
    if (ch - 1 > RC_MAX_CH || ch == 0)
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

static float _roll_rate, _pitch_rate, _yaw_rate;
static float _roll_angle, _pitch_angle;
static uint _roll_ch, _pitch_ch, _yaw_ch;

void RCMixerSetRates(
    float roll, // units of degrees per second.
    float pitch,
    float yaw)
{
    _roll_rate = roll;
    _pitch_rate = pitch;
    _yaw_rate = yaw;
}

void RCMixerSetAngleLimits(float roll, float pitch) // units of degrees.
{
    _roll_angle = roll;
    _pitch_angle = pitch;
}

void RCMixerSetAxisMapping(uint rollCh, uint pitchCh, uint yawCh)
{
    _roll_ch = rollCh;
    _pitch_ch = pitchCh;
    _yaw_ch = yawCh;
}

rates_t RCMixerGetRateSetpoints(RC_t inputRC)
{
    RCMixerRunInPlace(inputRC);

    rates_t out;
    out.roll = inputRC[_roll_ch] * _roll_rate;
    out.pitch = inputRC[_pitch_ch] * _pitch_rate;
    out.yaw = inputRC[_yaw_ch] * _yaw_rate;
    return out;
}

// The yaw output is a RATE!!
euler_t RCMixerGetAngleSetpoints(RC_t inputRC)
{
    RCMixerRunInPlace(inputRC);
    euler_t out;
    out.roll = inputRC[_roll_ch] * _roll_angle;
    out.pitch = inputRC[_pitch_ch] * _pitch_angle;
    out.yaw = inputRC[_yaw_ch] * _yaw_rate;
    return out;
}

// Primary Header
#include <picoflight/rc/rcMixer.h>

// Local Headers
#include <picoflight/rc/rc.h>
#include <picoflight/core/utilities.h>
#include <picoflight/orientation/types.h>

// pico-sdk Headers
#include <pico/types.h>

// Standard Headers
#include <stdbool.h>

static rcMixerChannelParameters_t _channel_params[RC_MAX_CH]; // Zero-indexed

void rcMixerInit(void)
{
    for (uint i = 0; i < RC_MAX_CH; ++i) {
        _channel_params[i].enabled = false;
    }
}

void rcMixerSetChannelParameters(uint ch, rcMixerChannelParameters_t params)
{
    if (ch - 1 > RC_MAX_CH || ch == 0)
        return;

    _channel_params[ch + 1] = params;
}

void rcMixerRunInPlace(rc_t channels)
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

void rcMixerSetRates(
    float roll, // units of degrees per second.
    float pitch,
    float yaw)
{
    _roll_rate = roll;
    _pitch_rate = pitch;
    _yaw_rate = yaw;
}

void rcMixerSetAngleLimits(float roll, float pitch) // units of degrees.
{
    _roll_angle = roll;
    _pitch_angle = pitch;
}

void rcMixerSetAxisMapping(uint rollCh, uint pitchCh, uint yawCh)
{
    _roll_ch = rollCh;
    _pitch_ch = pitchCh;
    _yaw_ch = yawCh;
}

bodyRates_t rcMixerGetRateSetpoints(rc_t inputRC)
{
    rcMixerRunInPlace(inputRC);

    bodyRates_t out;
    out.roll = inputRC[_roll_ch] * _roll_rate;
    out.pitch = inputRC[_pitch_ch] * _pitch_rate;
    out.yaw = inputRC[_yaw_ch] * _yaw_rate;
    return out;
}

// The yaw output is a RATE!!
bodyAngles_t rcMixerGetAngleSetpoints(rc_t inputRC)
{
    rcMixerRunInPlace(inputRC);
    bodyAngles_t out;
    out.roll = inputRC[_roll_ch] * _roll_angle;
    out.pitch = inputRC[_pitch_ch] * _pitch_angle;
    out.yaw = inputRC[_yaw_ch] * _yaw_rate;
    return out;
}

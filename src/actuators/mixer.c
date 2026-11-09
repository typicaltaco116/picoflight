// Primary Header
#include <picoflight/actuators/mixer.h>

// Local Headers
#include <picoflight/actuators/PWM.h>
#include <picoflight/orientation/types.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

static bool _armedFlag = false;
static char _pinoutStruct[sizeof(quad_pinout_t)];
static mixer_type_e _type;

void mixerQuadInit(quad_pinout_t *pinout)
{
    _armedFlag = false;
    _type = MIXER_QUADCOPTER;
    memcpy(_pinoutStruct, pinout, sizeof(quad_pinout_t));
}

void mixerFixedWingInit(fixed_wing_pinout_t *pinout)
{
    _armedFlag = false;
    _type = MIXER_FIXED_WING;
    memcpy(_pinoutStruct, pinout, sizeof(fixed_wing_pinout_t));
}

void mixerInit(mixer_type_e type, void *pinout)
{
    switch (type) {
        default:
        case MIXER_QUADCOPTER: mixerQuadInit(pinout); break;
        case MIXER_FIXED_WING: mixerFixedWingInit(pinout); break;
    }
}

// expects commands from -1 to 1
void mixerGetQuadOutputs(euler_t cmd, float throttle, float *outputs)
{
    float motor1, motor2, motor3, motor4;
    quad_pinout_t *pinout = (quad_pinout_t*)_pinoutStruct;

    if (_armedFlag) {
        throttle    = mapf(throttle, -1.0f, 1.0f, 0.0f, 1.0f);
        cmd.roll    = mapf(cmd.roll, -1.0f, 1.0f, 0.0f, 1.0f);
        cmd.pitch   = mapf(cmd.pitch, -1.0f, 1.0f, 0.0f, 1.0f);
        cmd.yaw     = mapf(cmd.yaw, -1.0f, 1.0f, 0.0f, 1.0f);

        motor1 = constrainf(throttle - cmd.pitch + cmd.roll + cmd.yaw, 0.0f, 1.0f);
        motor2 = constrainf(throttle - cmd.pitch - cmd.roll - cmd.yaw, 0.0f, 1.0f);
        motor3 = constrainf(throttle + cmd.pitch - cmd.roll + cmd.yaw, 0.0f, 1.0f);
        motor4 = constrainf(throttle + cmd.pitch + cmd.roll - cmd.yaw, 0.0f, 1.0f);
    } else {
        motor1 = motor2 = motor3 = motor4 = 0.0f;
    }

    outputs[pinout->m1 - 1] = mapf(motor1, 0.0f, 1.0f, -1.0f, 1.0f);
    outputs[pinout->m2 - 1] = mapf(motor2, 0.0f, 1.0f, -1.0f, 1.0f);
    outputs[pinout->m3 - 1] = mapf(motor3, 0.0f, 1.0f, -1.0f, 1.0f);
    outputs[pinout->m4 - 1] = mapf(motor4, 0.0f, 1.0f, -1.0f, 1.0f);
}

// expects command to be -1 to 1
void mixerGetFixedWingOutputs(euler_t cmd, float throttle, float *outputs)
{
    fixed_wing_pinout_t *pinout = (fixed_wing_pinout_t*)_pinoutStruct;

    if (!_armedFlag)
        throttle = -1.0f;

    outputs[pinout->throttle - 1]   = throttle;
    outputs[pinout->aileron - 1]    = cmd.roll;
    outputs[pinout->elevator - 1]   = cmd.pitch;
    outputs[pinout->rudder - 1]     = cmd.yaw;
}

// expects commands to be -1 to 1
void mixerGetOutputs(euler_t cmd, float throttle, float *outputs)
{
    switch (_type) {
        default:
        case MIXER_QUADCOPTER:
            mixerGetQuadOutputs(cmd, throttle, outputs);
            break;
        case MIXER_FIXED_WING:
            mixerGetFixedWingOutputs(cmd, throttle, outputs);
            break;
    }
}

void mixerSetArm(bool armed)
{
    _armedFlag = armed;
}

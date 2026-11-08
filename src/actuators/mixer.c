#include <picoflight/actuators/mixer.h>

#include <picoflight/actuators/PWM.h>
#include <picoflight/actuators/outputs.h>
#include <picoflight/orientation/types.h>

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

static bool _armedFlag = false;
static char _pinoutStruct[sizeof(quad_pinout_t)];
static mixer_type_e _type;

void mixerInit(mixer_type_e type, void *pinoutStruct)
{
    _armedFlag = false;
    _type = type;
    memcpy(_pinoutStruct, pinoutStruct, sizeof(quad_pinout_t));

    switch (_type) {
        default:
        case MIXER_QUADCOPTER: {
            quad_pinout_t *pinout = (quad_pinout_t*)_pinoutStruct;
            PWMInit(pinout->m1); PWMDrive(pinout->m1, -1.0f);
            PWMInit(pinout->m2); PWMDrive(pinout->m2, -1.0f);
            PWMInit(pinout->m3); PWMDrive(pinout->m3, -1.0f);
            PWMInit(pinout->m4); PWMDrive(pinout->m4, -1.0f);

            for (int32_t i = 0; i <= pinout->numAux; ++i) {
                PWMInit(pinout->aux[i]);
                PWMDrive(pinout->aux[i], 0.0f);
            }
            break;
        }

        case MIXER_FIXED_WING: {
            fixed_wing_pinout_t *pinout = (fixed_wing_pinout_t*)_pinoutStruct;
            PWMInit(pinout->aileron);   PWMDrive(pinout->aileron, 0.0f);
            PWMInit(pinout->elevator);  PWMDrive(pinout->elevator, 0.0f);
            PWMInit(pinout->throttle);  PWMDrive(pinout->throttle, -1.0f); 
            PWMInit(pinout->rudder);    PWMDrive(pinout->rudder, 0.0f);

            for (int32_t i = 0; i <= pinout->numAux; ++i){
                PWMInit(pinout->aux[i]);
                PWMDrive(pinout->aux[i], 0.0f);
            }
            break;
        }
    }
}

static float normalize_input(float input)
// Expects input values to be -1 to 1
{
    return (input + 1.0f) / 2.0f;
}

// expects commands from -1 to 1
static void quadcopter_mix_drive(euler_t cmd, float throttle)
{
    float motor1, motor2, motor3, motor4;

    if (_armedFlag) {
        throttle    = normalize_input(throttle);
        cmd.roll    = normalize_input(cmd.roll);
        cmd.pitch   = normalize_input(cmd.pitch);
        cmd.yaw     = normalize_input(cmd.yaw);

        motor1 = throttle - cmd.pitch + cmd.roll + cmd.yaw;
        motor2 = throttle - cmd.pitch - cmd.roll - cmd.yaw;
        motor3 = throttle + cmd.pitch - cmd.roll + cmd.yaw;
        motor4 = throttle + cmd.pitch + cmd.roll - cmd.yaw;
    } else {
        motor1 = motor2 = motor3 = motor4 = 0.0f;
    }

    PWMDriveNorm(((quad_pinout_t*)_pinoutStruct)->m1, motor1);
    PWMDriveNorm(((quad_pinout_t*)_pinoutStruct)->m2, motor1);
    PWMDriveNorm(((quad_pinout_t*)_pinoutStruct)->m3, motor1);
    PWMDriveNorm(((quad_pinout_t*)_pinoutStruct)->m4, motor1);
}

// expects command to be -1 to 1
static void fixedwing_mixing(euler_t cmd, float throttle)
{
    if (!_armedFlag)
        throttle = -1.0f;
        
    PWMDrive(
        ((fixed_wing_pinout_t*)_pinoutStruct)->throttle,
        throttle
    );
    PWMDrive(((fixed_wing_pinout_t*)_pinoutStruct)->aileron, cmd.roll);
    PWMDrive(((fixed_wing_pinout_t*)_pinoutStruct)->elevator, cmd.pitch);
    PWMDrive(((fixed_wing_pinout_t*)_pinoutStruct)->rudder, cmd.yaw);
}

// expects commands to be -1 to 1
void mixerDriveOutputs(euler_t input, float throttle)
{
    switch (_type) {
        default:
        case MIXER_QUADCOPTER:
            quadcopter_mix_drive(input, throttle);
            break;
        case MIXER_FIXED_WING:
            fixedwing_mixing(input, throttle);
            break;
    }
}

void mixerSetArm(bool armed)
{
    _armedFlag = armed;
}

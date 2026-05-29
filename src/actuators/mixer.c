#include "mixer.h"

#include "src/actuators/PWM.h"
#include "src/actuators/outputs.h"
#include "src/orientation/types.h"

#include <stdbool.h>
#include <stdint.h>

static bool _armed_status = false;
static PWM_t _pwm_outputs[OUTPUT_MAX_COUNT + 1];
static mixer_type_e _config;

static void drive_default_safe_values(mixer_type_e config)
{
    switch (config) {
        default:
        case MIXER_QUADCOPTER:
            PWM_Drive(_pwm_outputs[1], -1.0f);
            PWM_Drive(_pwm_outputs[2], -1.0f);
            PWM_Drive(_pwm_outputs[3], -1.0f);
            PWM_Drive(_pwm_outputs[4], -1.0f);

            for (int i = 5; i <= OUTPUT_MAX_COUNT; ++i)
                PWM_Drive(_pwm_outputs[i], 0.0f);
            break;

        case MIXER_FIXED_WING:
            for (int i = 1; i <= OUTPUT_MAX_COUNT; ++i) {
                if (i == 3)
                    PWM_Drive(_pwm_outputs[3], -1.0f);
                else
                    PWM_Drive(_pwm_outputs[i], 0.0f);
            }
            break;
    }
}

void mixer_Init(mixer_type_e config, output_pins_t out_pins)
{
    _armed_status = false;

    for (int i = 1; i <= OUTPUT_MAX_COUNT; ++i) {
        _pwm_outputs[i] = PWM_Register(out_pins[i]);
        PWM_Init(_pwm_outputs[i]);
    }

    _config = config;
    drive_default_safe_values(config);
}

static float normalize_input(float input)
// Expects input values to be -1 to 1
{
    return (input + 1.0f) / 2.0f;
}

static void quadcopter_mixing(euler_t input, float throttle)
{
    float motor1, motor2, motor3, motor4;

    if (_armed_status) {
        throttle    = normalize_input(throttle);
        input.pitch = normalize_input(input.pitch);
        input.roll  = normalize_input(input.roll);
        input.yaw   = normalize_input(input.yaw);

        motor1 = throttle - input.pitch + input.roll + input.yaw;
        motor2 = throttle - input.pitch - input.roll - input.yaw;
        motor3 = throttle + input.pitch - input.roll + input.yaw;
        motor4 = throttle + input.pitch + input.roll - input.yaw;
    } else {
        motor1 = motor2 = motor3 = motor4 = 0.0f;
    }

    PWM_DriveNorm(_pwm_outputs[1], motor1);
    PWM_DriveNorm(_pwm_outputs[2], motor2);
    PWM_DriveNorm(_pwm_outputs[3], motor3);
    PWM_DriveNorm(_pwm_outputs[4], motor4);
}

static void fixedwing_mixing(euler_t input, float throttle)
{
    PWM_Drive(_pwm_outputs[1], input.roll);
    PWM_Drive(_pwm_outputs[2], input.pitch);
    PWM_Drive(_pwm_outputs[4], input.yaw);

    if (_armed_status)
        PWM_DriveNorm(_pwm_outputs[3], normalize_input(throttle));
    else
        PWM_DriveNorm(_pwm_outputs[3], 0.0f);
}

void mixer_DriveOutputs(euler_t input, float throttle)
{
    switch (_config) {
        default:
        case MIXER_QUADCOPTER:
            quadcopter_mixing(input, throttle);
            break;
        case MIXER_FIXED_WING:
            fixedwing_mixing(input, throttle);
            break;
    }
}

void mixer_SetArm(bool armed)
{
    _armed_status = armed;
}

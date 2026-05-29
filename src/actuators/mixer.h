#pragma once

#include "src/actuators/outputs.h"
#include "src/orientation/types.h"

#include <stdbool.h>

typedef enum {
    MIXER_QUADCOPTER,
    MIXER_FIXED_WING,
} mixer_type_e;

void mixer_Init(mixer_type_e config, output_pins_t out_pins);

void mixer_TranslateAttitude(euler_t input);

void mixer_TranslateThrottle(float norm_throttle);

void mixer_DriveOutputs(euler_t input, float throttle);

void mixer_SetArm(bool armed);

#pragma once

#include <picoflight/actuators/outputs.h>
#include <picoflight/orientation/types.h>

#include <stdbool.h>

typedef enum {
    MIXER_QUADCOPTER,
    MIXER_FIXED_WING,
} mixer_type_e;

void mixerInit(mixer_type_e config, output_pins_t out_pins);

void mixerTranslateAttitude(euler_t input);

void mixerTranslateThrottle(float norm_throttle);

void mixerDriveOutputs(euler_t input, float throttle);

void mixerSetArm(bool armed);

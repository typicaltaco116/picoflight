#pragma once

#include <picoflight/actuators/outputs.h>
#include <picoflight/orientation/types.h>

#include <stdbool.h>

typedef enum {
    MIXER_QUADCOPTER,
    MIXER_FIXED_WING,
} mixer_type_e;

typedef struct {
    int32_t m1;
    int32_t m2;
    int32_t m3;
    int32_t m4;
    int32_t numAux;
    int32_t *aux; // this pointer must remain in scope!!
} quad_pinout_t;

typedef struct {
    int32_t aileron;
    int32_t elevator;
    int32_t throttle;
    int32_t rudder;
    int32_t numAux;
    int32_t *aux;
} fixed_wing_pinout_t;

void mixerInit(mixer_type_e type, void *pinoutStruct);

void mixerDriveOutputs(euler_t input, float throttle);

void mixerSetArm(bool armed);

#pragma once

// Local Headers
#include <picoflight/orientation/types.h>

// Standard Headers
#include <stdbool.h>
#include <stdint.h>

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

void mixerQuadInit(quad_pinout_t *pinout);

void mixerFixedWingInit(fixed_wing_pinout_t *pinout);

void mixerInit(mixer_type_e type, void *pinoutStruct);

// expects commands from -1 to 1
void mixerGetQuadOutputs(euler_t cmd, float throttle, float *outputs);

// expects command to be -1 to 1
void mixerGetFixedWingOutputs(euler_t cmd, float throttle, float *outputs);

// expects commands to be -1 to 1
void mixerGetOutputs(euler_t cmd, float throttle, float *outputs);

void mixerSetArm(bool armed);

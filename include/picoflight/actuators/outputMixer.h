#pragma once

// Local Headers
#include <picoflight/orientation/types.h>

// Standard Headers
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MIXER_QUADCOPTER,
    MIXER_FIXED_WING,
} outputMixerType_e;

typedef struct {
    int32_t m1;
    int32_t m2;
    int32_t m3;
    int32_t m4;
    int32_t numAux;
    int32_t *aux; // this pointer must remain in scope!!
} outputMixerQuadPinout_t;

typedef struct {
    int32_t aileron;
    int32_t elevator;
    int32_t throttle;
    int32_t rudder;
    int32_t numAux;
    int32_t *aux;
} outputMixerFixedWingPinout_t;

void outputMixerQuadInit(outputMixerQuadPinout_t *pinout);

void outputMixerFixedWingInit(outputMixerFixedWingPinout_t *pinout);

void outputMixerInit(outputMixerType_e type, void *pinoutStruct);

// expects commands from -1 to 1
void outputMixerGetQuadOutputs(bodyRates_t cmd, float throttle, float *outputs);

// expects command to be -1 to 1
void outputMixerGetFixedWingOutputs(bodyRates_t cmd, float throttle, float *outputs);

// expects commands to be -1 to 1
void outputMixerGetOutputs(bodyRates_t cmd, float throttle, float *outputs);

void outputMixerSetArm(bool armed);

#pragma once

// Local Headers
#include <picoflight/orientation/types.h>

// Standard Headers
#include <stdbool.h>

typedef struct {
    float p;
    float i;
    float d;
    float all;
} PID_gains_t;

void PIDCalcCommands(
        bool isAngle,
        euler_t platformAngles,
        rates_t platformRates,
        float dt,
        float rollSetpoint,
        float pitchSetpoint,
        float yawSetpoint, // always rate setpoint
        float *rollCommand,
        float *pitchCommand,
        float *yawCommand
);

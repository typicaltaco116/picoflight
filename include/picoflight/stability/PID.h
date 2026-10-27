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

typedef struct {
    float iMin;
    float iMax;
} PID_integral_bounds_t;

typedef struct {
    PID_gains_t k;
    PID_integral_bounds_t bounds;
} PID_params_t;

void PIDSetRateParameters(PID_params_t *roll, PID_params_t *pitch, PID_params_t *yaw);

void PIDSetAngleParameters(PID_params_t *roll, PID_params_t *pitch);

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

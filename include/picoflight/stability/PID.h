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
    PID_gains_t gains;
    PID_integral_bounds_t bounds;
    float accumulator;
} PID_loop_t;

PID_loop_t* PIDConstruct(PID_gains_t gains, PID_integral_bounds_t bounds);

PID_loop_t* PIDConstructFromFlash(
        const char *prefix,
        PID_gains_t defaultGains,
        PID_integral_bounds_t defaultBounds
);

void PIDFree(PID_loop_t *obj);

float PIDGetResponse(
        PID_loop_t *obj,
        float setpoint,
        float measurement,
        float measurementDot,
        float deltaTime
);

typedef struct {
    PID_loop_t *rollRateController;
    PID_loop_t *rollAngleController;
    PID_loop_t *pitchRateController;
    PID_loop_t *pitchAngleController;
    PID_loop_t *yawRateController;
    float rollDifferentiatorState;
    float pitchDifferentiatorState;
    float yawDifferentiatorState;
} PID_3_axis_t;

PID_3_axis_t* PID3AxisConstruct(
        PID_gains_t rollRateGains,
        PID_gains_t rollAngleGains,
        PID_gains_t pitchRateGains,
        PID_gains_t pitchAngleGains,
        PID_gains_t yawRateGains,
        PID_integral_bounds_t rollRateBounds,
        PID_integral_bounds_t rollAngleBounds,
        PID_integral_bounds_t pitchRateBounds,
        PID_integral_bounds_t pitchAngleBounds,
        PID_integral_bounds_t yawRateBounds
);

PID_3_axis_t* PID3AxisConstructFromFlash(
        PID_gains_t defualtRollRateGains,
        PID_gains_t defualtRollAngleGains,
        PID_gains_t defualtPitchRateGains,
        PID_gains_t defualtPitchAngleGains,
        PID_gains_t defualtYawRateGains,
        PID_integral_bounds_t defualtRollRateBounds,
        PID_integral_bounds_t defualtRollAngleBounds,
        PID_integral_bounds_t defualtPitchRateBounds,
        PID_integral_bounds_t defualtPitchAngleBounds,
        PID_integral_bounds_t defualtYawRateBounds
);

void PID3AxisFree(PID_3_axis_t *obj);

void PID3AxisGetAngleResponse(
        PID_3_axis_t *obj,
        platform_state_t state,
        float deltaTime,
        euler_t setpoints, // the yaw in this vector is a rate!!!!!!
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm
);

void PID3AxisGetRateResponse(
        PID_3_axis_t *obj,
        platform_state_t state,
        float deltaTime,
        rates_t setpoints,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm
);


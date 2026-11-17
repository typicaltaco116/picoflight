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
} PIDGains_t;

typedef struct {
    float iMin;
    float iMax;
} PIDIntegralLimits_t;

typedef struct {
    PIDGains_t gains;
    PIDIntegralLimits_t bounds;
    float accumulator;
} PIDLoop_t;

PIDLoop_t* PIDConstruct(PIDGains_t gains, PIDIntegralLimits_t bounds);

PIDLoop_t* PIDConstructFromFlash(
        const char *prefix,
        PIDGains_t defaultGains,
        PIDIntegralLimits_t defaultBounds
);

void PIDFree(PIDLoop_t *obj);

float PIDGetResponse(
        PIDLoop_t *obj,
        float setpoint,
        bool isArmed,
        float measurement,
        float measurementDot,
        float deltaTime
);

typedef struct {
    PIDLoop_t *rollRateController;
    PIDLoop_t *rollAngleController;
    PIDLoop_t *pitchRateController;
    PIDLoop_t *pitchAngleController;
    PIDLoop_t *yawRateController;
    float rollDifferentiatorState;
    float pitchDifferentiatorState;
    float yawDifferentiatorState;
} PID3Axis_t;

PID3Axis_t* PID3AxisConstruct(
        PIDGains_t rollRateGains,
        PIDGains_t rollAngleGains,
        PIDGains_t pitchRateGains,
        PIDGains_t pitchAngleGains,
        PIDGains_t yawRateGains,
        PIDIntegralLimits_t rollRateBounds,
        PIDIntegralLimits_t rollAngleBounds,
        PIDIntegralLimits_t pitchRateBounds,
        PIDIntegralLimits_t pitchAngleBounds,
        PIDIntegralLimits_t yawRateBounds
);

PID3Axis_t* PID3AxisConstructFromFlash(
        PIDGains_t defualtRollRateGains,
        PIDGains_t defualtRollAngleGains,
        PIDGains_t defualtPitchRateGains,
        PIDGains_t defualtPitchAngleGains,
        PIDGains_t defualtYawRateGains,
        PIDIntegralLimits_t defualtRollRateBounds,
        PIDIntegralLimits_t defualtRollAngleBounds,
        PIDIntegralLimits_t defualtPitchRateBounds,
        PIDIntegralLimits_t defualtPitchAngleBounds,
        PIDIntegralLimits_t defualtYawRateBounds
);

void PID3AxisFree(PID3Axis_t *obj);

void PID3AxisGetAngleResponse(
        PID3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        euler_t setpoints, // the yaw in this vector is a rate!!!!!!
        bool isArmed,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm
);

void PID3AxisGetRateResponse(
        PID3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        rates_t setpoints,
        bool isArmed,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm
);


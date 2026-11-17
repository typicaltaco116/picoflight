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
} pidGains_t;

typedef struct {
    float iMin;
    float iMax;
} pidIntegralLimits_t;

typedef struct {
    pidGains_t gains;
    pidIntegralLimits_t bounds;
    float accumulator;
} pidLoop_t;

pidLoop_t* pidConstruct(pidGains_t gains, pidIntegralLimits_t bounds);

pidLoop_t* pidConstructFromFlash(
        const char *prefix,
        pidGains_t defaultGains,
        pidIntegralLimits_t defaultBounds
);

void pidFree(pidLoop_t *obj);

float pidGetResponse(
        pidLoop_t *obj,
        float setpoint,
        bool isArmed,
        float measurement,
        float measurementDot,
        float deltaTime
);

typedef struct {
    pidLoop_t *rollRateController;
    pidLoop_t *rollAngleController;
    pidLoop_t *pitchRateController;
    pidLoop_t *pitchAngleController;
    pidLoop_t *yawRateController;
    float rollDifferentiatorState;
    float pitchDifferentiatorState;
    float yawDifferentiatorState;
} pid3Axis_t;

pid3Axis_t* pid3AxisConstruct(
        pidGains_t rollRateGains,
        pidGains_t rollAngleGains,
        pidGains_t pitchRateGains,
        pidGains_t pitchAngleGains,
        pidGains_t yawRateGains,
        pidIntegralLimits_t rollRateBounds,
        pidIntegralLimits_t rollAngleBounds,
        pidIntegralLimits_t pitchRateBounds,
        pidIntegralLimits_t pitchAngleBounds,
        pidIntegralLimits_t yawRateBounds
);

pid3Axis_t* pid3AxisConstructFromFlash(
        pidGains_t defualtRollRateGains,
        pidGains_t defualtRollAngleGains,
        pidGains_t defualtPitchRateGains,
        pidGains_t defualtPitchAngleGains,
        pidGains_t defualtYawRateGains,
        pidIntegralLimits_t defualtRollRateBounds,
        pidIntegralLimits_t defualtRollAngleBounds,
        pidIntegralLimits_t defualtPitchRateBounds,
        pidIntegralLimits_t defualtPitchAngleBounds,
        pidIntegralLimits_t defualtYawRateBounds
);

void pid3AxisFree(pid3Axis_t *obj);

void pid3AxisGetAngleResponse(
        pid3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        bodyAngles_t setpoints, // the yaw in this vector is a rate!!!!!!
        bool isArmed,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm
);

void pid3AxisGetRateResponse(
        pid3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        bodyRates_t setpoints,
        bool isArmed,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm
);

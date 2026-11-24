// Primary Header
#include <picoflight/stability/pid.h>

// Local Headers
#include <picoflight/orientation/types.h>
#include <picoflight/core/utilities.h>
#include <picoflight/core/configHelpers.h>

// pico-sdk Header
#include <pico/types.h>

// Standard Headers
#include <stdbool.h>
#include <stdlib.h>

pidLoop_t* pidConstruct(pidGains_t gains, pidIntegralLimits_t bounds)
{
    pidLoop_t *obj = malloc(sizeof(pidLoop_t));
    obj->gains = gains;
    obj->bounds = bounds;
    obj->accumulator = 0.0f;
    return obj;
}

pidLoop_t* pidConstructFromFlash(
        const char *prefix,
        pidGains_t defaultGains,
        pidIntegralLimits_t defaultBounds)
{
    pidGains_t gains = configGetIfPIDGains(prefix, defaultGains);
    pidIntegralLimits_t bounds = configGetIfPIDBounds(prefix, defaultBounds);

    return pidConstruct(gains, bounds);
}

void pidFree(pidLoop_t *obj)
{
    free(obj);
}


float pidGetResponse(
        pidLoop_t *obj,
        float setpoint,
        bool isArmed,
        float measurement,
        float measurementDot,
        float deltaTime)
{
    float error = setpoint - measurement;

    float p_term = obj->gains.p * error;

    float i_term = 0.0f;
    if (isArmed) {
        i_term = obj->gains.i * error * deltaTime + obj->accumulator;
        obj->accumulator += i_term;
    }

    float d_term = obj->gains.d * measurementDot;

    return constrainf(obj->gains.all * (p_term + i_term + d_term), -1.0f, 1.0f);
}


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
        pidIntegralLimits_t yawRateBounds)
{
    pid3Axis_t *obj = malloc(sizeof(pid3Axis_t));
    obj->rollRateController = pidConstruct(rollRateGains, rollRateBounds);
    obj->rollAngleController = pidConstruct(rollAngleGains, rollAngleBounds);
    obj->pitchRateController = pidConstruct(pitchRateGains, pitchRateBounds);
    obj->pitchAngleController = pidConstruct(pitchAngleGains, pitchAngleBounds);
    obj->yawRateController = pidConstruct(yawRateGains, yawRateBounds);
    obj->rollDifferentiatorState = 0.0f;
    obj->pitchDifferentiatorState = 0.0f;
    obj->yawDifferentiatorState = 0.0f;
    return obj;
}

pid3Axis_t* pid3AxisConstructFromFlash(
        pidGains_t rollRateGains,
        pidGains_t rollAngleGains,
        pidGains_t pitchRateGains,
        pidGains_t pitchAngleGains,
        pidGains_t yawRateGains,
        pidIntegralLimits_t rollRateBounds,
        pidIntegralLimits_t rollAngleBounds,
        pidIntegralLimits_t pitchRateBounds,
        pidIntegralLimits_t pitchAngleBounds,
        pidIntegralLimits_t yawRateBounds)
{
    pid3Axis_t *obj = malloc(sizeof(pid3Axis_t));
    obj->rollRateController = pidConstructFromFlash(
            "ROLL_RATE", rollRateGains, rollRateBounds);
    obj->rollAngleController = pidConstructFromFlash(
            "ROLL_ANGLE", rollAngleGains, rollAngleBounds);
    obj->pitchRateController = pidConstructFromFlash(
            "PITCH_RATE", pitchRateGains, pitchRateBounds);
    obj->pitchAngleController = pidConstructFromFlash(
            "PITCH_ANGLE", pitchAngleGains, pitchAngleBounds);
    obj->yawRateController = pidConstructFromFlash(
            "YAW_RATE", yawRateGains, yawRateBounds);
    obj->rollDifferentiatorState = 0.0f;
    obj->pitchDifferentiatorState = 0.0f;
    obj->yawDifferentiatorState = 0.0f;
    return obj;
}

void pid3AxisFree(pid3Axis_t *obj)
{
    pidFree(obj->rollRateController);
    pidFree(obj->rollAngleController);
    pidFree(obj->pitchRateController);
    pidFree(obj->pitchAngleController);
    pidFree(obj->yawRateController);
    free(obj);
}

bodyRates_t pid3AxisGetAngleResponse(
        pid3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        bodyAngles_t setpoints, // the yaw in this vector is a rate!!!!!!
        bool isArmed)
{
    bodyRates_t cmd;
    cmd.roll = pidGetResponse(
            obj->rollAngleController,
            setpoints.roll,
            isArmed,
            state.theta.roll,
            state.omega.roll,
            deltaTime
    );

    cmd.pitch = pidGetResponse(
            obj->pitchAngleController,
            setpoints.pitch,
            isArmed,
            state.theta.pitch,
            state.omega.pitch,
            deltaTime
    );

    float yawDotDot = (state.omega.yaw - obj->yawDifferentiatorState) / deltaTime;
    obj->yawDifferentiatorState = yawDotDot;
    cmd.yaw = pidGetResponse(
            obj->yawRateController,
            setpoints.yaw,
            isArmed,
            state.omega.yaw,
            yawDotDot,
            deltaTime
    );

    return cmd;
}

bodyRates_t pid3AxisGetRateResponse(
        pid3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        bodyRates_t setpoints,
        bool isArmed)
{
    bodyRates_t cmd;

    float rollDotDot = (state.omega.roll - obj->rollDifferentiatorState) / deltaTime;
    obj->rollDifferentiatorState = rollDotDot;
    cmd.roll = pidGetResponse(
            obj->rollAngleController,
            setpoints.roll,
            isArmed,
            state.omega.roll,
            rollDotDot,
            deltaTime
    );

    float pitchDotDot = (state.omega.pitch - obj->pitchDifferentiatorState) / deltaTime;
    obj->pitchDifferentiatorState = pitchDotDot;
    cmd.pitch = pidGetResponse(
            obj->pitchAngleController,
            setpoints.pitch,
            isArmed,
            state.omega.pitch,
            pitchDotDot,
            deltaTime
    );

    float yawDotDot = (state.omega.yaw - obj->yawDifferentiatorState) / deltaTime;
    obj->yawDifferentiatorState = yawDotDot;
    cmd.yaw = pidGetResponse(
            obj->yawRateController,
            setpoints.yaw,
            isArmed,
            state.omega.yaw,
            yawDotDot,
            deltaTime
    );

    return cmd;
}

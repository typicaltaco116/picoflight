// Primary Header
#include <picoflight/stability/PID.h>

// Local Headers
#include <picoflight/orientation/types.h>
#include <picoflight/core/utilities.h>
#include <picoflight/core/configHelpers.h>

// pico-sdk Header
#include <pico/types.h>

// Standard Headers
#include <stdbool.h>
#include <stdlib.h>

PIDLoop_t* PIDConstruct(PIDGains_t gains, PIDIntegralLimits_t bounds)
{
    PIDLoop_t *obj = malloc(sizeof(PIDLoop_t));
    obj->gains = gains;
    obj->bounds = bounds;
    obj->accumulator = 0.0f;
    return obj;
}

PIDLoop_t* PIDConstructFromFlash(
        const char *prefix,
        PIDGains_t defaultGains,
        PIDIntegralLimits_t defaultBounds)
{
    PIDGains_t gains = configGetIfPIDGains(prefix, defaultGains);
    PIDIntegralLimits_t bounds = configGetIfPIDBounds(prefix, defaultBounds);

    return PIDConstruct(gains, bounds);
}

void PIDFree(PIDLoop_t *obj)
{
    free(obj);
}


float PIDGetResponse(
        PIDLoop_t *obj,
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
        PIDIntegralLimits_t yawRateBounds)
{
    PID3Axis_t *obj = malloc(sizeof(PID3Axis_t));
    obj->rollRateController = PIDConstruct(rollRateGains, rollRateBounds);
    obj->rollAngleController = PIDConstruct(rollAngleGains, rollAngleBounds);
    obj->pitchRateController = PIDConstruct(pitchRateGains, pitchRateBounds);
    obj->pitchAngleController = PIDConstruct(pitchAngleGains, pitchAngleBounds);
    obj->yawRateController = PIDConstruct(yawRateGains, yawRateBounds);
    obj->rollDifferentiatorState = 0.0f;
    obj->pitchDifferentiatorState = 0.0f;
    obj->yawDifferentiatorState = 0.0f;
    return obj;
}

PID3Axis_t* PID3AxisConstructFromFlash(
        PIDGains_t rollRateGains,
        PIDGains_t rollAngleGains,
        PIDGains_t pitchRateGains,
        PIDGains_t pitchAngleGains,
        PIDGains_t yawRateGains,
        PIDIntegralLimits_t rollRateBounds,
        PIDIntegralLimits_t rollAngleBounds,
        PIDIntegralLimits_t pitchRateBounds,
        PIDIntegralLimits_t pitchAngleBounds,
        PIDIntegralLimits_t yawRateBounds)
{
    PID3Axis_t *obj = malloc(sizeof(PID3Axis_t));
    obj->rollRateController = PIDConstructFromFlash(
            "ROLL_RATE", rollRateGains, rollRateBounds);
    obj->rollAngleController = PIDConstructFromFlash(
            "ROLL_ANGLE", rollAngleGains, rollAngleBounds);
    obj->pitchRateController = PIDConstructFromFlash(
            "PITCH_RATE", pitchRateGains, pitchRateBounds);
    obj->pitchAngleController = PIDConstructFromFlash(
            "PITCH_ANGLE", pitchAngleGains, pitchAngleBounds);
    obj->yawRateController = PIDConstructFromFlash(
            "YAW_RATE", yawRateGains, yawRateBounds);
    obj->rollDifferentiatorState = 0.0f;
    obj->pitchDifferentiatorState = 0.0f;
    obj->yawDifferentiatorState = 0.0f;
    return obj;
}

void PID3AxisFree(PID3Axis_t *obj)
{
    PIDFree(obj->rollRateController);
    PIDFree(obj->rollAngleController);
    PIDFree(obj->pitchRateController);
    PIDFree(obj->pitchAngleController);
    PIDFree(obj->yawRateController);
    free(obj);
}

void PID3AxisGetAngleResponse(
        PID3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        euler_t setpoints, // the yaw in this vector is a rate!!!!!!
        bool isArmed,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm)
{
    *rollCommandNorm = PIDGetResponse(
            obj->rollAngleController,
            setpoints.roll,
            isArmed,
            state.theta.roll,
            state.omega.roll,
            deltaTime
    );

    *pitchCommandNorm = PIDGetResponse(
            obj->pitchAngleController,
            setpoints.pitch,
            isArmed,
            state.theta.pitch,
            state.omega.pitch,
            deltaTime
    );

    float yawDotDot = (state.omega.yaw - obj->yawDifferentiatorState) / deltaTime;
    obj->yawDifferentiatorState = yawDotDot;
    *yawCommandNorm = PIDGetResponse(
            obj->yawRateController,
            setpoints.yaw,
            isArmed,
            state.omega.yaw,
            yawDotDot,
            deltaTime
    );
}

void PID3AxisGetRateResponse(
        PID3Axis_t *obj,
        platformState_t state,
        float deltaTime,
        rates_t setpoints,
        bool isArmed,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm)
{
    float rollDotDot = (state.omega.roll - obj->rollDifferentiatorState) / deltaTime;
    obj->rollDifferentiatorState = rollDotDot;
    *rollCommandNorm = PIDGetResponse(
            obj->rollAngleController,
            setpoints.roll,
            isArmed,
            state.omega.roll,
            rollDotDot,
            deltaTime
    );

    float pitchDotDot = (state.omega.pitch - obj->pitchDifferentiatorState) / deltaTime;
    obj->pitchDifferentiatorState = pitchDotDot;
    *pitchCommandNorm = PIDGetResponse(
            obj->pitchAngleController,
            setpoints.pitch,
            isArmed,
            state.omega.pitch,
            pitchDotDot,
            deltaTime
    );

    float yawDotDot = (state.omega.yaw - obj->yawDifferentiatorState) / deltaTime;
    obj->yawDifferentiatorState = yawDotDot;
    *yawCommandNorm = PIDGetResponse(
            obj->yawRateController,
            setpoints.yaw,
            isArmed,
            state.omega.yaw,
            yawDotDot,
            deltaTime
    );
}

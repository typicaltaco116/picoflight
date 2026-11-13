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

PID_loop_t* PIDConstruct(PID_gains_t gains, PID_integral_bounds_t bounds)
{
    PID_loop_t *obj = malloc(sizeof(PID_loop_t));
    obj->gains = gains;
    obj->bounds = bounds;
    obj->accumulator = 0.0f;
    return obj;
}

PID_loop_t* PIDConstructFromFlash(
        const char *prefix,
        PID_gains_t defaultGains,
        PID_integral_bounds_t defaultBounds)
{
    PID_gains_t gains = configGetIfPIDGains(prefix, defaultGains);
    PID_integral_bounds_t bounds = configGetIfPIDBounds(prefix, defaultBounds);

    return PIDConstruct(gains, bounds);
}

void PIDFree(PID_loop_t *obj)
{
    free(obj);
}


float PIDGetResponse(
        PID_loop_t *obj,
        float setpoint,
        float measurement,
        float measurementDot,
        float deltaTime)
{
    float error = setpoint - measurement;

    float p_term = obj->gains.p * error;

    float i_term = obj->gains.i * error * deltaTime + obj->accumulator;
    obj->accumulator += i_term;

    float d_term = obj->gains.d * measurementDot;

    return constrainf(obj->gains.all * (p_term + i_term + d_term), -1.0f, 1.0f);
}


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
        PID_integral_bounds_t yawRateBounds)
{
    PID_3_axis_t *obj = malloc(sizeof(PID_3_axis_t));
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

PID_3_axis_t* PID3AxisConstructFromFlash(
        PID_gains_t rollRateGains,
        PID_gains_t rollAngleGains,
        PID_gains_t pitchRateGains,
        PID_gains_t pitchAngleGains,
        PID_gains_t yawRateGains,
        PID_integral_bounds_t rollRateBounds,
        PID_integral_bounds_t rollAngleBounds,
        PID_integral_bounds_t pitchRateBounds,
        PID_integral_bounds_t pitchAngleBounds,
        PID_integral_bounds_t yawRateBounds)
{
    PID_3_axis_t *obj = malloc(sizeof(PID_3_axis_t));
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

void PID3AxisFree(PID_3_axis_t *obj)
{
    PIDFree(obj->rollRateController);
    PIDFree(obj->rollAngleController);
    PIDFree(obj->pitchRateController);
    PIDFree(obj->pitchAngleController);
    PIDFree(obj->yawRateController);
    free(obj);
}

void PID3AxisGetAngleResponse(
        PID_3_axis_t *obj,
        platform_state_t state,
        float deltaTime,
        euler_t setpoints, // the yaw in this vector is a rate!!!!!!
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm)
{
    *rollCommandNorm = PIDGetResponse(
            obj->rollAngleController,
            setpoints.roll,
            state.theta.roll,
            state.omega.roll,
            deltaTime
    );

    *pitchCommandNorm = PIDGetResponse(
            obj->pitchAngleController,
            setpoints.pitch,
            state.theta.pitch,
            state.omega.pitch,
            deltaTime
    );

    float yawDotDot = (state.omega.yaw - obj->yawDifferentiatorState) / deltaTime;
    obj->yawDifferentiatorState = yawDotDot;
    *yawCommandNorm = PIDGetResponse(
            obj->yawRateController,
            setpoints.yaw,
            state.omega.yaw,
            yawDotDot,
            deltaTime
    );
}

void PID3AxisGetRateResponse(
        PID_3_axis_t *obj,
        platform_state_t state,
        float deltaTime,
        rates_t setpoints,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm)
{
    float rollDotDot = (state.omega.roll - obj->rollDifferentiatorState) / deltaTime;
    obj->rollDifferentiatorState = rollDotDot;
    *rollCommandNorm = PIDGetResponse(
            obj->rollAngleController,
            setpoints.roll,
            state.omega.roll,
            rollDotDot,
            deltaTime
    );

    float pitchDotDot = (state.omega.pitch - obj->pitchDifferentiatorState) / deltaTime;
    obj->pitchDifferentiatorState = pitchDotDot;
    *pitchCommandNorm = PIDGetResponse(
            obj->pitchAngleController,
            setpoints.pitch,
            state.omega.pitch,
            pitchDotDot,
            deltaTime
    );

    float yawDotDot = (state.omega.yaw - obj->yawDifferentiatorState) / deltaTime;
    obj->yawDifferentiatorState = yawDotDot;
    *yawCommandNorm = PIDGetResponse(
            obj->yawRateController,
            setpoints.yaw,
            state.omega.yaw,
            yawDotDot,
            deltaTime
    );
}

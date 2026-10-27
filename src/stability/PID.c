// Primary Header
#include <picoflight/stability/PID.h>

// Local Headers
#include <picoflight/orientation/types.h>

// pico-sdk Header
#include <pico/types.h>

// Standard Headers
#include <stdbool.h>

#define PARAMS_ROLL_ANGLE   (0)
#define PARAMS_PITCH_ANGLE  (1)
#define PARAMS_ROLL_RATE    (2)
#define PARAMS_PITCH_RATE   (3)
#define PARAMS_YAW_RATE     (4)
PID_params_t _params[5];

void PIDSetRateParameters(PID_params_t *roll, PID_params_t *pitch, PID_params_t *yaw)
{
    _params[PARAMS_ROLL_RATE] = *roll;
    _params[PARAMS_PITCH_RATE] = *pitch;
    _params[PARAMS_YAW_RATE] = *yaw;
}

void PIDSetAngleParameters(PID_params_t *roll, PID_params_t *pitch)
{
    _params[PARAMS_ROLL_ANGLE] = *roll;
    _params[PARAMS_PITCH_ANGLE] = *pitch;
}

static inline float singlePIDRun(
        uint paramIdx,
        float setpoint,
        float input,
        float dt,
        float *i_accumulator,
        float dxdt)
{
    PID_gains_t k = _params[paramIdx].k;

    float error = setpoint - input;

    float p_term = k.p * error;

    float i_term = k.i * error * dt + *i_accumulator;
    *i_accumulator += i_term;
    // Could also multiply the I term gain after its been integrated

    float d_term = k.d * dxdt;

    return k.all * (p_term + i_term + d_term);
}

static void PIDCalcCommandsAngle(
        euler_t platformAngles,
        rates_t platformRates,
        float dt,
        float rollSetpointNorm,
        float pitchSetpointNorm,
        float yawSetpointNorm,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm)
{
    (void)platformAngles; // surpress unused wanring for now
    
    static float roll_I = 0.0f;
    static float pitch_I = 0.0f;
    static float yaw_I = 0.0f;
    static float yaw_old = 0.0f;
    float yaw_dxdt;

    // Calculate the Angular Acceleration
    yaw_dxdt = (platformRates.yaw - yaw_old) / dt;
    yaw_old = platformRates.yaw;

    // Simple Angle Control
    *rollCommandNorm = singlePIDRun(PARAMS_ROLL_ANGLE, rollSetpointNorm, platformAngles.roll, dt, &roll_I, platformRates.roll);
    *pitchCommandNorm = singlePIDRun(PARAMS_PITCH_ANGLE, pitchSetpointNorm, platformAngles.pitch, dt, &pitch_I, platformRates.pitch);

    // Simple Rate Control
    *yawCommandNorm = singlePIDRun(PARAMS_YAW_RATE, yawSetpointNorm, platformRates.yaw, dt, &yaw_I, yaw_dxdt);
}


static void PIDCalcCommandsRate(
        rates_t platformRates,
        float dt,
        float rollSetpointNorm,
        float pitchSetpointNorm,
        float yawSetpointNorm,
        float *rollCommandNorm,
        float *pitchCommandNorm,
        float *yawCommandNorm)
{
    static float roll_I = 0.0f;
    static float pitch_I = 0.0f;
    static float yaw_I = 0.0f;
    static float roll_old = 0.0f;
    static float pitch_old = 0.0f;
    static float yaw_old = 0.0f;
    float roll_dxdt;
    float pitch_dxdt;
    float yaw_dxdt;

    // May differentiate the angle in order to have a second source in the future

    // Calculate the Angular Acceleration
    roll_dxdt = (platformRates.roll - roll_old) / dt;
    pitch_dxdt = (platformRates.pitch - pitch_old) / dt;
    yaw_dxdt = (platformRates.yaw - yaw_old) / dt;
    roll_old = platformRates.roll;
    pitch_old = platformRates.pitch;
    yaw_old = platformRates.yaw;

    // Simple Rate Controllers
    *rollCommandNorm = singlePIDRun(PARAMS_ROLL_RATE, rollSetpointNorm, platformRates.roll, dt, &roll_I, roll_dxdt);
    *pitchCommandNorm = singlePIDRun(PARAMS_PITCH_RATE, pitchSetpointNorm, platformRates.pitch, dt, &pitch_I, pitch_dxdt);
    *yawCommandNorm = singlePIDRun(PARAMS_YAW_RATE, yawSetpointNorm, platformRates.yaw, dt, &yaw_I, yaw_dxdt);
}

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
        float *yawCommand)
{
    if (isAngle) {
        PIDCalcCommandsAngle(platformAngles, platformRates, dt, rollSetpoint,
                pitchSetpoint, yawSetpoint, rollCommand, pitchCommand, yawCommand);
    }
    else {
        PIDCalcCommandsRate(platformRates, dt, rollSetpoint,
                pitchSetpoint, yawSetpoint, rollCommand, pitchCommand, yawCommand);
    }
}

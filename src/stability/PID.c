// Primary Header
#include <picoflight/stability/PID.h>

// Local Headers
#include <picoflight/orientation/types.h>

// Standard Headers
#include <stdbool.h>

PID_gains_t _k_roll_angle, _k_pitch_angle;
PID_gains_t _k_roll_rate, _k_pitch_rate, _k_yaw_rate;

void PIDInit(void)
{
}

void PIDSetGains(
    PID_gains_t roll_angle,
    PID_gains_t pitch_angle,
    PID_gains_t roll_rate,
    PID_gains_t pitch_rate,
    PID_gains_t yaw_rate)
{
    _k_roll_angle = roll_angle;
    _k_pitch_angle = pitch_angle;
    _k_roll_rate = roll_rate;
    _k_pitch_rate = pitch_rate;
    _k_yaw_rate = yaw_rate;
}

static float singleLoopRun(
        PID_gains_t K,
        float setpoint,
        float input,
        float dt,
        float *i_accumulator,
        float dxdt)
{
    float error, command, p_term, i_term, d_term;

    error = setpoint - input;

    p_term = K.p * error;

    i_term = K.i * error * dt + *i_accumulator;
    *i_accumulator += i_term;
    // Could also multiply the I term gain after its been integrated

    d_term = K.d * dxdt;

    command = K.all * (p_term + i_term + d_term);
    return command;
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
    *rollCommandNorm = singleLoopRun(_k_roll_angle, rollSetpointNorm, platformAngles.roll, dt, &roll_I, platformRates.roll);
    *pitchCommandNorm = singleLoopRun(_k_pitch_angle, pitchSetpointNorm, platformAngles.pitch, dt, &pitch_I, platformRates.pitch);

    // Simple Rate Control
    *yawCommandNorm = singleLoopRun(_k_yaw_rate, yawSetpointNorm, platformRates.yaw, dt, &yaw_I, yaw_dxdt);
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
    *rollCommandNorm = singleLoopRun(_k_roll_rate, rollSetpointNorm, platformRates.roll, dt, &roll_I, roll_dxdt);
    *pitchCommandNorm = singleLoopRun(_k_pitch_rate, pitchSetpointNorm, platformRates.pitch, dt, &pitch_I, pitch_dxdt);
    *yawCommandNorm = singleLoopRun(_k_yaw_rate, yawSetpointNorm, platformRates.yaw, dt, &yaw_I, yaw_dxdt);
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

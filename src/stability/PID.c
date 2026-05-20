#include "PID.h"

#inclde "src/orientation/types.h"

static float singleLoopRun(PID_gains_t gain,
                           float setpoint, float input,
                           float dt, float *i_accumulator,
                           float dxdt)
{
    float error, command, p_term, i_term, d_term;

    error = input - setpoint;

    p_term = gain.k_P * error;

    i_term = gain.k_I * error * dt + *i_accumulator;
    *i_accumulator += i_term;
    // Could also multiply the I term gain after its been integrated

    d_term = gain.k_D * dxdt;

    command = gain.k_All * (p_term + i_term + d_term);
    return command;
}


void PID_ThreeAxisAngleControl(
    PID_gains_t roll, PID_gains_t pitch, PID_gains_t yaw,
    rates_t rates, euler_t angles, float dt,
    float roll_set, float pitch_set, float yaw_set,
    float *roll_cmd, float *pitch_cmd, float *yaw_cmd)
{
    static float roll_I = 0.0f;
    static float pitch_I = 0.0f;
    static float yaw_I = 0.0f;
    static float yaw_old = 0.0f;
    float yaw_dxdt;

    yaw_dxdt = (rates.yaw - yaw_old) / dt;
    yaw_old = rates.yaw;

    // Simple Angle Control
    singleLoopRun(roll, roll_set, angles.roll, dt, &roll_I, rates.roll);

    // Simple Angle Control
    singleLoopRun(pitch, pitch_set, angles.pitch, dt, &pitch_I, rates.pitch);

    // Simple Rate Control
    singleLoopRun(yaw, yaw_set, rates.yaw, dt, &yaw_I, yaw_dxdt);
}


void PID_ThreeAxisRateControl(
    PID_gains_t roll, PID_gains_t pitch, PID_gains_t yaw,
    rates_t rates, euler_t angles, float dt,
    float roll_set, float pitch_set, float yaw_set,
    float *roll_cmd, float *pitch_cmd, float *yaw_cmd)
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
    roll_dxdt = (rates.roll - roll_old) / dt;
    pitch_dxdt = (rates.pitch - pitch_old) / dt;
    yaw_dxdt = (rates.yaw - yaw_old) / dt;
    roll_old = rates.roll;
    pitch_old = rates.pitch;
    yaw_old = rates.yaw;

    // Simple Rate Controllers
    singleLoopRun(roll, roll_set, rates.roll, dt, &roll_I, roll_dxdt);
    singleLoopRun(pitch, pitch_set, rates.pitch, dt, &pitch_I, pitch_dxdt);
    singleLoopRun(yaw, yaw_set, rates.yaw, dt, &yaw_I, yaw_dxdt);
}

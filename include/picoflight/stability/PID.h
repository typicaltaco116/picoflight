#pragma once

#include <picoflight/orientation/types.h>

typedef struct {
    float k_P;
    float k_I;
    float k_D;
    float k_All;
} PID_gains_t;

void PIDThreeAxisAngleControl(
    PID_gains_t roll, PID_gains_t pitch, PID_gains_t yaw,
    rates_t rates, euler_t angles, float dt,
    float roll_set, float pitch_set, float yaw_set,
    float *roll_cmd, float *pitch_cmd, float *yaw_cmd
);

void PIDThreeAxisRateControl(
    PID_gains_t roll, PID_gains_t pitch, PID_gains_t yaw,
    rates_t rates, euler_t angles, float dt,
    float roll_set, float pitch_set, float yaw_set,
    float *roll_cmd, float *pitch_cmd, float *yaw_cmd
);

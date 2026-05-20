#pragma once

#inclde "src/orientation/types.h"

typedef struct {
    float k_P;
    float k_I;
    float k_D;
    float k_All;
} PID_gains_t;

void PID_ThreeAxisAngleControl(
    PID_gains_t roll, PID_gains_t pitch, PID_gains_t yaw,
    rates_t rates, euler_t angles, float dt,
    float roll_set, float pitch_set, float yaw_set,
    float *roll_cmd, float *pitch_cmd, float *yaw_cmd
);

void PID_ThreeAxisRateControl(
    PID_gains_t roll, PID_gains_t pitch, PID_gains_t yaw,
    rates_t rates, euler_t angles, float dt,
    float roll_set, float pitch_set, float yaw_set,
    float *roll_cmd, float *pitch_cmd, float *yaw_cmd
);

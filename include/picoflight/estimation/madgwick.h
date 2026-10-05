#pragma once

#include <picoflight/orientation/types.h>
#include <picoflight/sensors/IMU_processing.h>

void madgwick_SetStepSize(float B_val);

void ComputeMadgwick(euler_t *angles, IMU_vectors_t imu, float sampleFreq);

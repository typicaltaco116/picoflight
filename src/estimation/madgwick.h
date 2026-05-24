#pragma once

#include "src/orientation/types.h"
#include "src/sensors/IMU_processing.h"

void madgwick_SetStepSize(float B_val);

void ComputeMadgwick(euler_t *angles, IMU_vectors_t imu, float sampleFreq);

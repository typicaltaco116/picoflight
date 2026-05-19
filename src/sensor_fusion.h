#pragma once

#include "src/orientation_types.h"
#include "src/IMU_processing.h"

void ComputeMadgwick(euler_t *angles, IMU_vectors_t imu, float sampleFreq);

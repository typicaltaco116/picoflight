#pragma once

#include "orientation_types.h"
#include "IMU_processing.h"

void ComputeMadgwick(euler_t *angles, IMU_vectors_t imu, float sampleFreq);

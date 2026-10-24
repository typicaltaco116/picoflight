#pragma once

#include <picoflight/orientation/types.h>
#include <picoflight/sensors/IMU_processing.h>

void madgwickSetStepSize(float B_val);

void madgwickCompute(euler_t *angles, IMU_vectors_t imu, float sampleFreq);

#pragma once

#include <picoflight/orientation/types.h>
#include <picoflight/sensors/IMU_processing.h>

void madgwickSetStepSize(float B_val);

platform_state_t madgwickCompute(IMU_vectors_t imu, float sampleFreq);

#pragma once

#include <picoflight/orientation/types.h>
#include <picoflight/sensors/IMU_processing.h>

void madgwickSetStepSize(float B_val);

platformState_t madgwickCompute(IMUVectors_t imu, float sampleFreq);

#pragma once

#include <picoflight/orientation/types.h>
#include <picoflight/sensors/imu.h>

void madgwickSetStepSize(float B_val);

platformState_t madgwickCompute(imuVectors_t imu, float sampleFreq);

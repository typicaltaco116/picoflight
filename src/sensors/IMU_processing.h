#pragma once

#include <stdint.h>

#include "src/sensors/IMU.h"

#define IMU_CALIBRATION_CYCLES 20000.0 // must be double literal

typedef struct {
    float x;
    float y;
    float z;
} IMU_accel_t;

typedef struct {
    float x;
    float y;
    float z;
} IMU_gyro_t;

typedef struct {
    IMU_gyro_t gyro;
    IMU_gyro_t accel;
} IMU_vectors_t;

IMU_vectors_t IMU_GetData(void);

void IMU_SetErrorOffset(IMU_vectors_t error_offset);

IMU_vectors_t IMU_GetCalibrationError(uint32_t cycle_count); 

void IMU_SetProcessingScale(IMU_gyro_scale_e g_scale, IMU_accel_scale_e a_scale);

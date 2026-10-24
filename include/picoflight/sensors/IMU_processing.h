#pragma once

#include <stdint.h>

#include <picoflight/sensors/IMU.h>

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

IMU_vectors_t IMUGetData(void);

void IMUSetErrorOffset(IMU_vectors_t error_offset);

IMU_vectors_t IMUGetCalibrationError(uint32_t cycle_count); 

void IMUSetProcessingScale(IMU_gyro_scale_e g_scale, IMU_accel_scale_e a_scale);

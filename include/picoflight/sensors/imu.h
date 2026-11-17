#pragma once

// Local Headers
#include <picoflight/sensors/mpu6050.h>

// Standard Headers
#include <stdint.h>

#define IMU_CALIBRATION_CYCLES 20000.0 // must be double literal

typedef struct {
    float x;
    float y;
    float z;
} accel_t;

typedef struct {
    float x;
    float y;
    float z;
} gyro_t;

typedef struct {
    gyro_t gyro;
    gyro_t accel;
} imuVectors_t;

imuVectors_t imuGetData(void);

void imuSetErrorOffset(imuVectors_t error_offset);

imuVectors_t imuGetCalibrationError(uint32_t cycle_count); 

void imuSetProcessingScale(mpu6050GyroScale_e g_scale, mpu6050AccelScale_e a_scale);

void imuLoadSaveCalibrationBlocking(uint32_t cycles);

bool imuGetIsCalibrated(void);

void imuLoadCalibration(void);

void imuSaveCalibrationBlocking(uint32_t cycles);

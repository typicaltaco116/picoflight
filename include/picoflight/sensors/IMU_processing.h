#pragma once

// Local Headers
#include <picoflight/sensors/IMU.h>

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
} IMUVectors_t;

IMUVectors_t IMUGetData(void);

void IMUSetErrorOffset(IMUVectors_t error_offset);

IMUVectors_t IMUGetCalibrationError(uint32_t cycle_count); 

void IMUSetProcessingScale(IMUGyroScale_e g_scale, IMUAccelScale_e a_scale);

void IMULoadSaveCalibrationBlocking(uint32_t cycles);

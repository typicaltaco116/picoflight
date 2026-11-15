#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <hardware/i2c.h>

typedef enum {
    IMU_5HZ_RATE,
    IMU_1KHZ_RATE,
} IMUSampleRate_e;

typedef enum {
    IMU_GYRO_SCALE_250,
    IMU_GYRO_SCALE_500,
    IMU_GYRO_SCALE_1000,
    IMU_GYRO_SCALE_2000,
} IMUGyroScale_e;

typedef enum {
    IMU_ACCEL_SCALE_2,
    IMU_ACCEL_SCALE_4,
    IMU_ACCEL_SCALE_8,
    IMU_ACCEL_SCALE_16,
} IMUAccelScale_e;

void IMUInitBus(i2c_inst_t *i2c, uint32_t sda_pin, uint32_t scl_pin, uint32_t speed);

void IMUInitDevice(IMUSampleRate_e rate, IMUGyroScale_e g_scale, IMUAccelScale_e a_scale);

void IMURegisterWrite(uint8_t address, uint8_t data);
uint8_t IMURegisterRead(uint8_t address);
void IMUBufferRead(uint8_t start_address, uint8_t *buffer, size_t length);

void IMUGetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z);

bool IMUIsDataReady(void);

IMUSampleRate_e IMUGetCurrentRate(void);
IMUGyroScale_e IMUGetCurrentGyroScale(void);
IMUAccelScale_e IMU_GetCurrentAccelScale(void);

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hardware/i2c.h"

typedef enum {
    IMU_5HZ_RATE,
    IMU_1KHZ_RATE,
} IMU_sample_rate_e;

typedef enum {
    IMU_GYRO_SCALE_250,
    IMU_GYRO_SCALE_500,
    IMU_GYRO_SCALE_1000,
    IMU_GYRO_SCALE_2000,
} IMU_gyro_scale_e;

typedef enum {
    IMU_ACCEL_SCALE_2,
    IMU_ACCEL_SCALE_4,
    IMU_ACCEL_SCALE_8,
    IMU_ACCEL_SCALE_16,
} IMU_accel_scale_e;

void IMU_InitBus(i2c_inst_t *i2c, uint32_t sda_pin, uint32_t scl_pin, uint32_t speed);

void IMU_InitDevice(IMU_sample_rate_e rate, IMU_gyro_scale_e g_scale, IMU_accel_scale_e a_scale);

void IMU_RegisterWrite(uint8_t address, uint8_t data);
uint8_t IMU_RegisterRead(uint8_t address);
void IMU_BufferRead(uint8_t start_address, uint8_t *buffer, size_t length);

void IMU_GetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z);

bool IMU_IsDataReady(void);

IMU_sample_rate_e IMU_GetCurrentRate(void);
IMU_gyro_scale_e IMU_GetCurrentGyroScale(void);
IMU_accel_scale_e IMU_GetCurrentAccelScale(void);

#pragma once

// Pico-SDK Headers
#include <hardware/i2c.h>

// Standard Headers
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    MPU6050_5HZ_RATE,
    MPU6050_1KHZ_RATE,
} mpu6050SampleRate_e;

typedef enum {
    MPU6050_GYRO_SCALE_250,
    MPU6050_GYRO_SCALE_500,
    MPU6050_GYRO_SCALE_1000,
    MPU6050_GYRO_SCALE_2000,
} mpu6050GyroScale_e;

typedef enum {
    MPU6050_ACCEL_SCALE_2,
    MPU6050_ACCEL_SCALE_4,
    MPU6050_ACCEL_SCALE_8,
    MPU6050_ACCEL_SCALE_16,
} mpu6050AccelScale_e;

void mpu6050InitBus(i2c_inst_t *i2c, uint32_t sda_pin, uint32_t scl_pin, uint32_t speed);

void mpu6050InitDevice(mpu6050SampleRate_e rate, mpu6050GyroScale_e g_scale, mpu6050AccelScale_e a_scale);

void mpu6050RegisterWrite(uint8_t address, uint8_t data);
uint8_t mpu6050RegisterRead(uint8_t address);
void mpu6050BufferRead(uint8_t start_address, uint8_t *buffer, size_t length);

void mpu6050GetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z);

bool mpu6050IsDataReady(void);

mpu6050SampleRate_e mpu6050GetCurrentRate(void);
mpu6050GyroScale_e mpu6050GetCurrentGyroScale(void);
mpu6050AccelScale_e mpu6050GetCurrentAccelScale(void);

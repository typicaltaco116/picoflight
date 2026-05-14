#include "IMU.h"

#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "MPU6050_register_map.h"

void IMU_RegisterWrite(uint8_t address, uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = address;
    buffer[1] = data;
    i2c_write_blocking(i2c_default, MPU6050_I2C_ADDR, buffer, 2, false);
}

uint8_t IMU_Read(uint8_t address)
{
    uint8_t value;
    i2c_write_blocking(i2c_default, MPU6050_I2C_ADDR, (uint8_t[]){address}, 1, true);
    i2c_read_blocking(i2c_default, MPU6050_I2C_ADDR, &value, 1, false);
    return value;
}

void IMU_BufferRead(uint8_t start_address, uint8_t *buffer, size_t length)
{
    i2c_write_blocking(i2c_default, MPU6050_I2C_ADDR, (uint8_t[]){start_address}, 1, true);
    i2c_read_blocking(i2c_default, MPU6050_I2C_ADDR, buffer, length, false);
}

void IMU_Init(void)
{
    // Reset IMU_ device
    IMU_RegisterWrite(MPU6050_PWR_MGMT_1, MPU6050_RESET_BIT);
    sleep_ms(100);

    // Clear Sleep Bit
    IMU_RegisterWrite(MPU6050_PWR_MGMT_1, 0x00);
    sleep_ms(10);

    // Set Sensor Sensitivity
    IMU_RegisterWrite(MPU6050_GYRO_CONFIG, MPU6050_SET_GYRO_SCALE_250);
    IMU_RegisterWrite(MPU6050_ACCEL_CONFIG, MPU6050_SET_ACCEL_SCALE_2);

    // Enable Digital Lowpass Filter
    IMU_RegisterWrite(MPU6050_CONFIG, MPU6050_DLPF_CFG_6);

    // Set Sample Rate Divider for 5Hz Sample Rate
    IMU_RegisterWrite(MPU6050_SMPLRT_DIV, 200-1);

    // Enable Data Ready Interrupt
    IMU_RegisterWrite(MPU6050_INT_ENABLE, MPU6050_DATA_RDY_INT_BIT);
}

void IMU_GetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z)
{
    uint8_t buffer[6];

    IMU_BufferRead(MPU6050_GYRO_XOUT_H, (uint8_t*)buffer, 6);

    // Need to swap placement since RP2350 is little endian and MPU6050 outputs
    // as big endian.
    *((uint8_t*)gyro_x + 0) = *(buffer + 1);
    *((uint8_t*)gyro_x + 1) = *(buffer + 0);
    *((uint8_t*)gyro_y + 0) = *(buffer + 3);
    *((uint8_t*)gyro_y + 1) = *(buffer + 2);
    *((uint8_t*)gyro_z + 0) = *(buffer + 5);
    *((uint8_t*)gyro_z + 1) = *(buffer + 4);

    IMU_BufferRead(MPU6050_ACCEL_XOUT_H, (uint8_t*)buffer, 6);

    *((uint8_t*)accel_x + 0) = *(buffer + 1);
    *((uint8_t*)accel_x + 1) = *(buffer + 0);
    *((uint8_t*)accel_y + 0) = *(buffer + 3);
    *((uint8_t*)accel_y + 1) = *(buffer + 2);
    *((uint8_t*)accel_z + 0) = *(buffer + 5);
    *((uint8_t*)accel_z + 1) = *(buffer + 4);
}

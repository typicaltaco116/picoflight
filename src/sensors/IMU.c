#include "IMU.h"

#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "src/sensors/IMU_processing.h"
#include "src/sensors/MPU6050_register_map.h"

static i2c_inst_t *IMU_i2c_instance;
static IMU_sample_rate_e sample_rate;
static IMU_gyro_scale_e gyro_scale;
static IMU_accel_scale_e accel_scale;


void IMU_InitBus(i2c_inst_t *i2c, uint32_t sda_pin, uint32_t scl_pin, uint32_t speed)
{
    IMU_i2c_instance = i2c;

    i2c_init(IMU_i2c_instance, speed); 

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

}


void IMU_InitDevice(IMU_sample_rate_e rate, IMU_gyro_scale_e g_scale, IMU_accel_scale_e a_scale)
{
    uint8_t gyro_config_value, accel_config_value, divider_value;

    sample_rate = rate;
    gyro_scale = g_scale;
    accel_scale = a_scale;

    IMU_SetProcessingScale(g_scale, a_scale);

    // Reset IMU_ device
    IMU_RegisterWrite(MPU6050_PWR_MGMT_1, MPU6050_RESET_BIT);
    sleep_ms(100);

    // Clear Sleep Bit
    IMU_RegisterWrite(MPU6050_PWR_MGMT_1, 0x00);
    sleep_ms(10);

    switch(gyro_scale) {
        default:
        case IMU_GYRO_SCALE_250:
            gyro_config_value = MPU6050_SET_GYRO_SCALE_250;
            break;
        case IMU_GYRO_SCALE_500:
            gyro_config_value = MPU6050_SET_GYRO_SCALE_500;
            break;
        case IMU_GYRO_SCALE_1000:
            gyro_config_value = MPU6050_SET_GYRO_SCALE_1000;
            break;
        case IMU_GYRO_SCALE_2000:
            gyro_config_value = MPU6050_SET_GYRO_SCALE_2000;
            break;
    }
    IMU_RegisterWrite(MPU6050_GYRO_CONFIG, gyro_config_value);

    switch(accel_scale) {
        default:
        case IMU_ACCEL_SCALE_2:
            accel_config_value = MPU6050_SET_ACCEL_SCALE_2;
            break;
        case IMU_ACCEL_SCALE_4:
            accel_config_value = MPU6050_SET_ACCEL_SCALE_4;
            break;
        case IMU_ACCEL_SCALE_8:
            accel_config_value = MPU6050_SET_ACCEL_SCALE_8;
            break;
        case IMU_ACCEL_SCALE_16:
            accel_config_value = MPU6050_SET_ACCEL_SCALE_16;
            break;
    }
    IMU_RegisterWrite(MPU6050_ACCEL_CONFIG, accel_config_value);

    // Enable Digital Lowpass Filter
    IMU_RegisterWrite(MPU6050_CONFIG, MPU6050_DLPF_CFG_6);

    switch(sample_rate) {
        case IMU_5HZ_RATE:
            divider_value = 200-1;
            break;
        default:
        case IMU_1KHZ_RATE:
            divider_value = 0; // no division as this is max speed
            break;
    }
    IMU_RegisterWrite(MPU6050_SMPLRT_DIV, divider_value);

    // Enable Data Ready Interrupt
    IMU_RegisterWrite(MPU6050_INT_ENABLE, MPU6050_DATA_RDY_INT_BIT);
}


void IMU_RegisterWrite(uint8_t address, uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = address;
    buffer[1] = data;
    i2c_write_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, buffer, 2, false);
}


uint8_t IMU_RegisterRead(uint8_t address)
{
    uint8_t value;
    i2c_write_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, (uint8_t[]){address}, 1, true);
    i2c_read_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, &value, 1, false);
    return value;
}


void IMU_BufferRead(uint8_t start_address, uint8_t *buffer, size_t length)
{
    i2c_write_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, (uint8_t[]){start_address}, 1, true);
    i2c_read_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, buffer, length, false);
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


bool IMU_IsDataReady(void)
{
    return (IMU_RegisterRead(MPU6050_INT_STATUS) & MPU6050_DATA_RDY_INT_BIT);
}


IMU_sample_rate_e IMU_GetCurrentRate(void)
{
    return sample_rate;
}

IMU_gyro_scale_e IMU_GetCurrentGyroScale(void)
{
    return gyro_scale;
}

IMU_accel_scale_e IMU_GetCurrentAccelScale(void)
{
    return accel_scale;
}

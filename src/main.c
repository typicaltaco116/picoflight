#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "sensors/MPU6050_register_map.h"
#include "sensors/IMU.h"

static float accel_x_offset;
static float accel_y_offset;
static float accel_z_offset;

int main() 
{
    stdio_init_all();

    IMU_InitBus(i2c_default, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, 200*1000);
    IMU_InitDevice(IMU_5HZ_RATE, IMU_GYRO_SCALE_250, IMU_ACCEL_SCALE_2);

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    while (1) {
        int16_t gyro_x, gyro_y, gyro_z;
        int16_t accel_x, accel_y, accel_z;
        float a_x, a_y, a_z;

        while (stdio_getchar_timeout_us(100) != (int)'\r');

        while (IMU_IsDataReady());

        IMU_GetRawData(&gyro_x, &gyro_y, &gyro_z, &accel_x, &accel_y, &accel_z);
        a_x = accel_x / MPU6050_ACCEL_2_LSB_SENS;
        a_y = accel_y / MPU6050_ACCEL_2_LSB_SENS;
        a_z = accel_z / MPU6050_ACCEL_2_LSB_SENS;

        printf("A = {%01.03f %01.03f %01.03f}\r\n", a_x, a_y, a_z);
    }
}

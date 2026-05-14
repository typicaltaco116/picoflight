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

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    i2c_init(i2c_default, 200 * 1000); // setup with 200kHz clock speed
    
    IMU_Init();

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    while (1) {
        int16_t gyro_x, gyro_y, gyro_z;
        int16_t accel_x, accel_y, accel_z;
        float a_x, a_y, a_z;

        while (stdio_getchar_timeout_us(100) != (int)'\r');

        while (~IMU_Read(MPU6050_INT_STATUS) & MPU6050_DATA_RDY_INT_BIT);

        IMU_GetRawData(&gyro_x, &gyro_y, &gyro_z, &accel_x, &accel_y, &accel_z);
        a_x = accel_x / MPU6050_ACCEL_2_LSB_SENS;
        a_y = accel_y / MPU6050_ACCEL_2_LSB_SENS;
        a_z = accel_z / MPU6050_ACCEL_2_LSB_SENS;

        printf("A = {%01.03f %01.03f %01.03f}\r\n", a_x, a_y, a_z);
    }
}

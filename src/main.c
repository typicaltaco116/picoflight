#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "sensors/IMU.h"
#include "IMU_processing.h"

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
        IMU_vectors_t IMU_data;

        while (stdio_getchar_timeout_us(100) != (int)'\r');

        while (IMU_IsDataReady());

        IMU_data = IMU_GetData();

        printf("A = {%01.03f %01.03f %01.03f}\r\n", 
               IMU_data.accel.x, IMU_data.accel.y, IMU_data.accel.z);
    }
}
